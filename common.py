#!/usr/bin/env python3

import argparse
import base64
import collections
import copy
import datetime
import distutils.file_util
import glob
import imp
import itertools
import json
import multiprocessing
import os
import re
import shlex
import shutil
import signal
import stat
import subprocess
import sys
import time
import urllib
import urllib.request

this_module = sys.modules[__name__]
repo_short_id = 'lkmc'
# https://stackoverflow.com/questions/20010199/how-to-determine-if-a-process-runs-inside-lxc-docker
in_docker = os.path.exists('/.dockerenv')
root_dir = os.path.dirname(os.path.abspath(__file__))
data_dir = os.path.join(root_dir, 'data')
p9_dir = os.path.join(data_dir, '9p')
gem5_non_default_src_root_dir = os.path.join(data_dir, 'gem5')
if in_docker:
    out_dir = os.path.join(root_dir, 'out.docker')
else:
    out_dir = os.path.join(root_dir, 'out')
bench_boot = os.path.join(out_dir, 'bench-boot.txt')
packages_dir = os.path.join(root_dir, 'buildroot_packages')
kernel_modules_subdir = 'kernel_modules'
kernel_modules_src_dir = os.path.join(this_module.root_dir, this_module.kernel_modules_subdir)
userland_subdir = 'userland'
userland_src_dir = os.path.join(this_module.root_dir, this_module.userland_subdir)
userland_build_ext = '.out'
include_subdir = 'include'
include_src_dir = os.path.join(this_module.root_dir, this_module.include_subdir)
submodules_dir = os.path.join(root_dir, 'submodules')
buildroot_src_dir = os.path.join(submodules_dir, 'buildroot')
crosstool_ng_src_dir = os.path.join(submodules_dir, 'crosstool-ng')
crosstool_ng_supported_archs = set(['arm', 'aarch64'])
linux_src_dir = os.path.join(submodules_dir, 'linux')
linux_config_dir = os.path.join(this_module.root_dir, 'linux_config')
rootfs_overlay_dir = os.path.join(this_module.root_dir, 'rootfs_overlay')
extract_vmlinux = os.path.join(linux_src_dir, 'scripts', 'extract-vmlinux')
qemu_src_dir = os.path.join(submodules_dir, 'qemu')
parsec_benchmark_src_dir = os.path.join(submodules_dir, 'parsec-benchmark')
ccache_dir = os.path.join('/usr', 'lib', 'ccache')
default_build_id = 'default'
arch_short_to_long_dict = collections.OrderedDict([
    ('x', 'x86_64'),
    ('a', 'arm'),
    ('A', 'aarch64'),
])
all_archs = [arch_short_to_long_dict[k] for k in arch_short_to_long_dict]
arch_choices = []
for key in this_module.arch_short_to_long_dict:
    arch_choices.append(key)
    arch_choices.append(this_module.arch_short_to_long_dict[key])
default_arch = 'x86_64'
gem5_cpt_prefix = '^cpt\.'
sha = subprocess.check_output(['git', '-C', root_dir, 'log', '-1', '--format=%H']).decode().rstrip()
release_dir = os.path.join(this_module.out_dir, 'release')
release_zip_file = os.path.join(this_module.release_dir, 'lkmc-{}.zip'.format(this_module.sha))
github_repo_id = 'cirosantilli/linux-kernel-module-cheat'
asm_ext = '.S'
c_ext = '.c'
header_ext = '.h'
kernel_module_ext = '.ko'
obj_ext = '.o'
config_file = os.path.join(data_dir, 'config')
command_prefix = '+ '
magic_fail_string = b'lkmc_test_fail'
if os.path.exists(config_file):
    config = imp.load_source('config', config_file)
    configs = {x:getattr(config, x) for x in dir(config) if not x.startswith('__')}

class Component:
    def __init__(self):
        pass

    def build(self):
        '''
        Parse CLI, and to the build based on it.

        The actual build work is done by do_build in implementing classes.
        '''
        parser = this_module.get_argparse(
            argparse_args=self.get_argparse_args(),
            default_args=self.get_default_args(),
        )
        self.add_parser_arguments(parser)
        parser.add_argument(
            '--clean',
            help='Clean the build instead of building.',
            action='store_true',
        )
        parser.add_argument(
            '-j', '--nproc',
            help='Number of processors to use for the build. Default: use all cores.',
            type=int,
            default=multiprocessing.cpu_count(),
        )
        args = this_module.setup(parser)
        if not this_module.dry_run:
            start_time = time.time()
        if args.clean:
            self.clean(args)
        else:
            self.do_build(args)
        if not this_module.dry_run:
            end_time = time.time()
            this_module.print_time(end_time - start_time)

    def add_parser_arguments(self, parser):
        pass

    def clean(self, args):
        build_dir = self.get_build_dir(args)
        if build_dir is not None:
            this_module.rmrf(build_dir)

    def do_build(self, args):
        '''
        Do the actual main build work.
        '''
        raise NotImplementedError()

    def get_argparse_args(self):
        '''
        Extra arguments for argparse.ArgumentParser.
        '''
        return {}

    def get_build_dir(self, args):
        '''
        Build directory, gets cleaned by --clean if not None.
        '''
        return None

    def get_default_args(self):
        '''
        Default values for command line arguments.
        '''
        return {}

class Newline:
    '''
    Singleton class. Can be used in print_cmd to print out nicer command lines
    with -key on the same line as "-key value".
    '''
    pass

def add_dry_run_argument(parser):
    parser.add_argument('--dry-run', default=False, action='store_true', help='''\
Print the commands that would be run, but don't run them.

We aim display every command that modifies the filesystem state, and generate
Bash equivalents even for actions taken directly in Python without shelling out.

mkdir are generally omitted since those are obvious.
''')

def add_newlines(cmd):
    out = []
    for arg in cmd:
        out.extend([arg, this_module.Newline])
    return out

def assert_crosstool_ng_supports_arch(arch):
    if arch not in this_module.crosstool_ng_supported_archs:
        raise Exception('arch not yet supported: ' + arch)

def base64_encode(string):
    return base64.b64encode(string.encode()).decode()

def write_string_to_file(path, string, mode='w'):
    global this_module
    if mode == 'a':
        redirect = '>>'
    else:
        redirect = '>'
    print_cmd("cat << 'EOF' {} {}\n{}\nEOF".format(redirect, path, string))
    if not this_module.dry_run:
        with open(path, 'a') as f:
            f.write(string)

def copy_dir_if_update_non_recursive(srcdir, destdir, filter_ext=None):
    # TODO print rsync equivalent.
    os.makedirs(destdir, exist_ok=True)
    for basename in os.listdir(srcdir):
        src = os.path.join(srcdir, basename)
        if os.path.isfile(src):
            noext, ext = os.path.splitext(basename)
            if filter_ext is not None and ext == filter_ext:
                distutils.file_util.copy_file(
                    src,
                    os.path.join(destdir, basename),
                    update=1,
                )

def gem_list_checkpoint_dirs():
    '''
    List checkpoint directory, oldest first.
    '''
    global this_module
    prefix_re = re.compile(this_module.gem5_cpt_prefix)
    files = list(filter(lambda x: os.path.isdir(os.path.join(this_module.m5out_dir, x)) and prefix_re.search(x), os.listdir(this_module.m5out_dir)))
    files.sort(key=lambda x: os.path.getmtime(os.path.join(this_module.m5out_dir, x)))
    return files

def get_argparse(default_args=None, argparse_args=None):
    '''
    Return an argument parser with common arguments set.

    :type default_args: Dict[str,str]
    :type argparse_args: Dict
    '''
    global this_module
    if default_args is None:
        default_args = {}
    if argparse_args is None:
        argparse_args = {}
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter,
        **argparse_args
    )
    this_module.add_dry_run_argument(parser)
    parser.add_argument(
        '-a', '--arch', choices=this_module.arch_choices, default=this_module.default_arch,
        help='CPU architecture. Default: %(default)s'
    )
    parser.add_argument(
        '-b', '--baremetal',
        help='''\
Use the given baremetal executable instead of the Linux kernel.

If the path is absolute, it is used as is.

If the path is relative, we assume that it points to a source code
inside baremetal/ and then try to use corresponding executable.
'''
    )
    parser.add_argument(
        '--buildroot-build-id',
        default=default_build_id,
        help='Buildroot build ID. Allows you to keep multiple separate gem5 builds. Default: %(default)s'
    )
    parser.add_argument(
        '--buildroot-linux', default=False, action='store_true',
        help='Boot with the Buildroot Linux kernel instead of our custom built one. Mostly for sanity checks.'
    )
    parser.add_argument(
        '--crosstool-ng-build-id', default=default_build_id,
        help='Crosstool-NG build ID. Allows you to keep multiple separate crosstool-NG builds. Default: %(default)s'
    )
    parser.add_argument(
        '--docker', default=False, action='store_true',
        help='''\
Use the docker download Ubuntu root filesystem instead of the default Buildroot one.
'''
    )
    parser.add_argument(
        '-g', '--gem5', default=False, action='store_true',
        help='Use gem5 instead of QEMU'
    )
    parser.add_argument(
        '--gem5-src',
        help='''\
Use the given directory as the gem5 source tree. Ignore `--gem5-worktree`.
'''
    )
    parser.add_argument(
        '-L', '--linux-build-id', default=default_build_id,
        help='Linux build ID. Allows you to keep multiple separate Linux builds. Default: %(default)s'
    )
    parser.add_argument(
        '--machine',
        help='''Machine type.
QEMU default: virt
gem5 default: VExpress_GEM5_V1
See the documentation for other values known to work.
'''
    )
    parser.add_argument(
        '-M', '--gem5-build-id',
        help='''\
gem5 build ID. Allows you to keep multiple separate gem5 builds. Default: {}
'''.format(default_build_id)
    )
    parser.add_argument(
        '-N', '--gem5-worktree',
        help='''\
Create and use a git worktree of the gem5 submodule.
See: https://github.com/cirosantilli/linux-kernel-module-cheat#gem5-worktree
'''
    )
    parser.add_argument(
        '--gem5-build-dir',
        help='''\
Use the given directory as the gem5 build directory.
'''
    )
    parser.add_argument(
        '--gem5-source-dir',
        help='''\
Use the given directory as the gem5 source tree. Ignore `--gem5-worktree`.
'''
    )
    parser.add_argument(
        '-n', '--run-id', default='0',
        help='''\
ID for run outputs such as gem5's m5out. Allows you to do multiple runs,
and then inspect separate outputs later in different output directories.
Default: %(default)s
'''
    )
    parser.add_argument(
        '-P', '--prebuilt', default=False, action='store_true',
        help='''\
Use prebuilt packaged host utilities as much as possible instead
of the ones we built ourselves. Saves build time, but decreases
the likelihood of incompatibilities.
'''
    )
    parser.add_argument(
        '--port-offset', type=int,
        help='''\
Increase the ports to be used such as for GDB by an offset to run multiple
instances in parallel.
Default: the run ID (-n) if that is an integer, otherwise 0.
'''
    )
    parser.add_argument(
        '-Q', '--qemu-build-id', default=default_build_id,
        help='QEMU build ID. Allows you to keep multiple separate QEMU builds. Default: %(default)s'
    )
    parser.add_argument(
        '--gem5-build-type', default='opt',
        help='gem5 build type, most often used for "debug" builds. Default: %(default)s'
    )
    parser.add_argument(
        '--userland-build-id', default=None
    )
    parser.add_argument(
        '-v', '--verbose', default=False, action='store_true',
        help='Show full compilation commands when they are not shown by default.'
    )
    if hasattr(this_module, 'configs'):
        defaults = this_module.configs.copy()
    else:
        defaults = {}
    defaults.update(default_args)
    # A bit ugly as it actually changes the defaults shown on --help, but we can't do any better
    # because it is impossible to check if arguments were given or not...
    # - https://stackoverflow.com/questions/30487767/check-if-argparse-optional-argument-is-set-or-not
    # - https://stackoverflow.com/questions/3609852/which-is-the-best-way-to-allow-configuration-options-be-overridden-at-the-comman
    parser.set_defaults(**defaults)
    return parser

def get_elf_entry(elf_file_path):
    global this_module
    readelf_header = subprocess.check_output([
        this_module.get_toolchain_tool('readelf'),
        '-h',
        elf_file_path
    ])
    for line in readelf_header.decode().split('\n'):
        split = line.split()
        if line.startswith('  Entry point address:'):
            addr = line.split()[-1]
            break
    return int(addr, 0)

def get_stats(stat_re=None, stats_file=None):
    global this_module
    if stat_re is None:
        stat_re = '^system.cpu[0-9]*.numCycles$'
    if stats_file is None:
        stats_file = this_module.stats_file
    stat_re = re.compile(stat_re)
    ret = []
    with open(stats_file, 'r') as statfile:
        for line in statfile:
            if line[0] != '-':
                cols = line.split()
                if len(cols) > 1 and stat_re.search(cols[0]):
                    ret.append(cols[1])
    return ret

def get_toolchain_prefix(tool, allowed_toolchains=None):
    buildroot_full_prefix = os.path.join(this_module.host_bin_dir, this_module.buildroot_toolchain_prefix)
    buildroot_exists = os.path.exists('{}-{}'.format(buildroot_full_prefix, tool))
    crosstool_ng_full_prefix = os.path.join(this_module.crosstool_ng_bin_dir, this_module.crosstool_ng_toolchain_prefix)
    crosstool_ng_exists = os.path.exists('{}-{}'.format(crosstool_ng_full_prefix, tool))
    host_tool = '{}-{}'.format(this_module.ubuntu_toolchain_prefix, tool)
    host_path = shutil.which(host_tool)
    if host_path is not None:
        host_exists = True
        host_full_prefix = host_path[:-(len(tool)+1)]
    else:
        host_exists = False
        host_full_prefix = None
    known_toolchains = {
        'crosstool-ng': (crosstool_ng_exists, crosstool_ng_full_prefix),
        'buildroot': (buildroot_exists, buildroot_full_prefix),
        'host': (host_exists, host_full_prefix),
    }
    if allowed_toolchains is None:
        if this_module.baremetal is None:
            allowed_toolchains = ['buildroot', 'crosstool-ng', 'host']
        else:
            allowed_toolchains = ['crosstool-ng', 'buildroot', 'host']
    tried = []
    for toolchain in allowed_toolchains:
        exists, prefix = known_toolchains[toolchain]
        tried.append('{}-{}'.format(prefix, tool))
        if exists:
            return prefix
    raise Exception('Tool not found. Tried:\n' + '\n'.join(tried))

def get_toolchain_tool(tool, allowed_toolchains=None):
    return '{}-{}'.format(this_module.get_toolchain_prefix(tool, allowed_toolchains), tool)

def github_make_request(
        authenticate=False,
        data=None,
        extra_headers=None,
        path='',
        subdomain='api',
        url_params=None,
        **extra_request_args
    ):
    global this_module
    if extra_headers is None:
        extra_headers = {}
    headers = {'Accept': 'application/vnd.github.v3+json'}
    headers.update(extra_headers)
    if authenticate:
        headers['Authorization'] = 'token ' + os.environ['LKMC_GITHUB_TOKEN']
    if url_params is not None:
        path += '?' + urllib.parse.urlencode(url_params)
    request = urllib.request.Request(
        'https://' + subdomain + '.github.com/repos/' + github_repo_id + path,
        headers=headers,
        data=data,
        **extra_request_args
    )
    response_body = urllib.request.urlopen(request).read().decode()
    if response_body:
        _json = json.loads(response_body)
    else:
        _json = {}
    return _json

def log_error(msg):
    print('error: {}'.format(msg), file=sys.stderr)

def make_build_dirs():
    global this_module
    os.makedirs(this_module.buildroot_build_build_dir, exist_ok=True)
    os.makedirs(this_module.gem5_build_dir, exist_ok=True)
    os.makedirs(this_module.out_rootfs_overlay_dir, exist_ok=True)

def make_run_dirs():
    '''
    Make directories required for the run.
    The user could nuke those anytime between runs to try and clean things up.
    '''
    global this_module
    os.makedirs(this_module.gem5_run_dir, exist_ok=True)
    os.makedirs(this_module.p9_dir, exist_ok=True)
    os.makedirs(this_module.qemu_run_dir, exist_ok=True)

def cmd_to_string(cmd, cwd=None, extra_env=None, extra_paths=None):
    '''
    Format a command given as a list of strings so that it can
    be viewed nicely and executed by bash directly and print it to stdout.
    '''
    last_newline = ' \\\n'
    newline_separator = last_newline + '  '
    out = []
    if extra_env is None:
        extra_env = {}
    if cwd is not None:
        out.append('cd {} &&'.format(shlex.quote(cwd)))
    if extra_paths is not None:
        out.append('PATH="{}:${{PATH}}"'.format(':'.join(extra_paths)))
    for key in extra_env:
        out.append('{}={}'.format(shlex.quote(key), shlex.quote(extra_env[key])))
    cmd_quote = []
    newline_count = 0
    for arg in cmd:
        if arg == this_module.Newline:
            cmd_quote.append(arg)
            newline_count += 1
        else:
            cmd_quote.append(shlex.quote(arg))
    if newline_count > 0:
        cmd_quote = [' '.join(list(y)) for x, y in itertools.groupby(cmd_quote, lambda z: z == this_module.Newline) if not x]
    out.extend(cmd_quote)
    if newline_count == 1 and cmd[-1] == this_module.Newline:
        ending = ''
    else:
        ending = last_newline + ';'
    return newline_separator.join(out) + ending

def print_cmd(cmd, cwd=None, cmd_file=None, extra_env=None, extra_paths=None):
    '''
    Print cmd_to_string to stdout.

    Optionally save the command to cmd_file file, and add extra_env
    environment variables to the command generated.

    If cmd contains at least one common.Newline, newlines are only added on common.Newline.
    Otherwise, newlines are added automatically after every word.
    '''
    global dry_run
    if type(cmd) is str:
        cmd_string = cmd
    else:
        cmd_string = cmd_to_string(cmd, cwd=cwd, extra_env=extra_env, extra_paths=extra_paths)
    print(this_module.command_prefix + cmd_string)
    if cmd_file is not None:
        with open(cmd_file, 'w') as f:
            f.write('#!/usr/bin/env bash\n')
            f.write(cmd_string)
        st = os.stat(cmd_file)
        os.chmod(cmd_file, st.st_mode | stat.S_IXUSR)

def print_time(ellapsed_seconds):
    hours, rem = divmod(ellapsed_seconds, 3600)
    minutes, seconds = divmod(rem, 60)
    print("time {:02}:{:02}:{:02}".format(int(hours), int(minutes), int(seconds)))

def raw_to_qcow2(prebuilt=False, reverse=False):
    global this_module
    if prebuilt:
        qemu_img_executable = this_module.qemu_img_basename
    else:
        qemu_img_executable = this_module.qemu_img_executable
    infmt = 'raw'
    outfmt = 'qcow2'
    infile = this_module.rootfs_raw_file
    outfile = this_module.qcow2_file
    if reverse:
        tmp = infmt
        infmt = outfmt
        outfmt = tmp
        tmp = infile
        infile = outfile
        outfile = tmp
    this_module.run_cmd([
        qemu_img_executable, this_module.Newline,
        # Prevent qemu-img from generating trace files like QEMU. Disgusting.
        '-T', 'pr_manager_run,file=/dev/null', this_module.Newline,
        'convert', this_module.Newline,
        '-f', infmt, this_module.Newline,
        '-O', outfmt, this_module.Newline,
        infile, this_module.Newline,
        outfile, this_module.Newline,
    ])

def resolve_args(defaults, args, extra_args):
    if extra_args is None:
        extra_args = {}
    argcopy = copy.copy(args)
    argcopy.__dict__ = dict(list(defaults.items()) + list(argcopy.__dict__.items()) + list(extra_args.items()))
    return argcopy

def cp(src, dest):
    global this_module
    print_cmd(['cp', src, dest])
    if not this_module.dry_run:
        shutil.copy2(src, dest)

def rmrf(path):
    print_cmd(['rm', '-r', '-f', path])
    if not this_module.dry_run and os.path.exists(path):
        shutil.rmtree(path)

def run_cmd(
        cmd,
        cmd_file=None,
        out_file=None,
        show_stdout=True,
        show_cmd=True,
        extra_env=None,
        extra_paths=None,
        delete_env=None,
        dry_run=False,
        raise_on_failure=True,
        **kwargs
    ):
    '''
    Run a command. Write the command to stdout before running it.

    Wait until the command finishes execution.

    :param cmd: command to run. common.Newline entries are magic get skipped.
    :type cmd: List[str]

    :param cmd_file: if not None, write the command to be run to that file
    :type cmd_file: str

    :param out_file: if not None, write the stdout and stderr of the command the file
    :type out_file: str

    :param show_stdout: wether to show stdout and stderr on the terminal or not
    :type show_stdout: bool

    :param extra_env: extra environment variables to add when running the command
    :type extra_env: Dict[str,str]

    :param dry_run: don't run the commands, just potentially print them. Debug aid.
    :type dry_run: Bool
    '''
    if out_file is not None:
        stdout = subprocess.PIPE
        stderr = subprocess.STDOUT
    else:
        if show_stdout:
            stdout = None
            stderr = None
        else:
            stdout = subprocess.DEVNULL
            stderr = subprocess.DEVNULL
    if extra_env is None:
        extra_env = {}
    if delete_env is None:
        delete_env = []
    if 'cwd' in kwargs:
        cwd = kwargs['cwd']
    else:
        cwd = None
    env = os.environ.copy()
    env.update(extra_env)
    if extra_paths is not None:
        path = ':'.join(extra_paths)
        if 'PATH' in os.environ:
            path += ':' + os.environ['PATH']
        env['PATH'] = path
    for key in delete_env:
        if key in env:
            del env[key]
    if show_cmd:
        print_cmd(cmd, cwd=cwd, cmd_file=cmd_file, extra_env=extra_env, extra_paths=extra_paths)

    # Otherwise Ctrl + C gives:
    # - ugly Python stack trace for gem5 (QEMU takes over terminal and is fine).
    # - kills Python, and that then kills GDB: https://stackoverflow.com/questions/19807134/does-python-always-raise-an-exception-if-you-do-ctrlc-when-a-subprocess-is-exec
    sigint_old = signal.getsignal(signal.SIGINT)
    signal.signal(signal.SIGINT, signal.SIG_IGN)

    # Otherwise BrokenPipeError when piping through | grep
    # But if I do this_module, my terminal gets broken at the end. Why, why, why.
    # https://stackoverflow.com/questions/14207708/ioerror-errno-32-broken-pipe-python
    # Ignoring the exception is not enough as it prints a warning anyways.
    #sigpipe_old = signal.getsignal(signal.SIGPIPE)
    #signal.signal(signal.SIGPIPE, signal.SIG_DFL)

    cmd = this_module.strip_newlines(cmd)
    if not dry_run and not this_module.dry_run:
        # https://stackoverflow.com/questions/15535240/python-popen-write-to-stdout-and-log-file-simultaneously/52090802#52090802
        with subprocess.Popen(cmd, stdout=stdout, stderr=stderr, env=env, **kwargs) as proc:
            if out_file is not None:
                os.makedirs(os.path.split(os.path.abspath(out_file))[0], exist_ok=True)
                with open(out_file, 'bw') as logfile:
                    while True:
                        byte = proc.stdout.read(1)
                        if byte:
                            if show_stdout:
                                sys.stdout.buffer.write(byte)
                                try:
                                    sys.stdout.flush()
                                except BlockingIOError:
                                    # TODO understand. Why, Python, why.
                                    pass
                            logfile.write(byte)
                        else:
                            break
        signal.signal(signal.SIGINT, sigint_old)
        #signal.signal(signal.SIGPIPE, sigpipe_old)
        returncode = proc.returncode
        if returncode != 0 and raise_on_failure:
            raise Exception('Command exited with status: {}'.format(returncode))
        return returncode
    else:
        return 0

def setup(parser):
    '''
    Parse the command line arguments, and setup several variables based on them.
    Typically done after getting inputs from the command line arguments.
    '''
    global this_module
    args = parser.parse_args()
    if args.arch in this_module.arch_short_to_long_dict:
        args.arch = this_module.arch_short_to_long_dict[args.arch]
    if args.gem5_build_id is None:
        args.gem5_build_id = default_build_id
        gem5_build_id_given = False
    else:
        gem5_build_id_given = True
    if args.userland_build_id is None:
        args.userland_build_id = default_build_id
        this_module.userland_build_id_given = False
    else:
        this_module.userland_build_id_given = True
    if args.gem5_worktree is not None and not gem5_build_id_given:
        args.gem5_build_id = args.gem5_worktree
    this_module.machine = args.machine
    this_module.setup_dry_run_arguments(args)
    this_module.is_arm = False
    if args.arch == 'arm':
        this_module.armv = 7
        this_module.gem5_arch = 'ARM'
        this_module.mcpu = 'cortex-a15'
        this_module.buildroot_toolchain_prefix = 'arm-buildroot-linux-uclibcgnueabihf'
        this_module.crosstool_ng_toolchain_prefix = 'arm-unknown-eabi'
        this_module.ubuntu_toolchain_prefix = 'arm-linux-gnueabihf'
        if args.gem5:
            if this_module.machine is None:
                this_module.machine = 'VExpress_GEM5_V1'
        else:
            if this_module.machine is None:
                this_module.machine = 'virt'
        this_module.is_arm = True
    elif args.arch == 'aarch64':
        this_module.armv = 8
        this_module.gem5_arch = 'ARM'
        this_module.mcpu = 'cortex-a57'
        this_module.buildroot_toolchain_prefix = 'aarch64-buildroot-linux-uclibc'
        this_module.crosstool_ng_toolchain_prefix = 'aarch64-unknown-elf'
        this_module.ubuntu_toolchain_prefix = 'aarch64-linux-gnu'
        if args.gem5:
            if this_module.machine is None:
                this_module.machine = 'VExpress_GEM5_V1'
        else:
            if this_module.machine is None:
                this_module.machine = 'virt'
        this_module.is_arm = True
    elif args.arch == 'x86_64':
        this_module.crosstool_ng_toolchain_prefix = 'x86_64-unknown-elf'
        this_module.gem5_arch = 'X86'
        this_module.buildroot_toolchain_prefix = 'x86_64-buildroot-linux-uclibc'
        this_module.ubuntu_toolchain_prefix = 'x86_64-linux-gnu'
        if args.gem5:
            if this_module.machine is None:
                this_module.machine = 'TODO'
        else:
            if this_module.machine is None:
                this_module.machine = 'pc'
    this_module.buildroot_out_dir = os.path.join(this_module.out_dir, 'buildroot')
    this_module.buildroot_build_dir = os.path.join(this_module.buildroot_out_dir, 'build', args.buildroot_build_id, args.arch)
    this_module.buildroot_download_dir = os.path.join(this_module.buildroot_out_dir, 'download')
    this_module.buildroot_config_file = os.path.join(this_module.buildroot_build_dir, '.config')
    this_module.buildroot_build_build_dir = os.path.join(this_module.buildroot_build_dir, 'build')
    this_module.buildroot_linux_build_dir = os.path.join(this_module.buildroot_build_build_dir, 'linux-custom')
    this_module.buildroot_vmlinux = os.path.join(this_module.buildroot_linux_build_dir, "vmlinux")
    this_module.qemu_build_dir = os.path.join(this_module.out_dir, 'qemu', args.qemu_build_id)
    this_module.qemu_executable_basename = 'qemu-system-{}'.format(args.arch)
    this_module.qemu_executable = os.path.join(this_module.qemu_build_dir, '{}-softmmu'.format(args.arch), this_module.qemu_executable_basename)
    this_module.qemu_img_basename = 'qemu-img'
    this_module.qemu_img_executable = os.path.join(this_module.qemu_build_dir, this_module.qemu_img_basename)
    this_module.host_dir = os.path.join(this_module.buildroot_build_dir, 'host')
    this_module.host_bin_dir = os.path.join(this_module.host_dir, 'usr', 'bin')
    this_module.buildroot_pkg_config = os.path.join(this_module.host_bin_dir, 'pkg-config')
    this_module.buildroot_images_dir = os.path.join(this_module.buildroot_build_dir, 'images')
    this_module.buildroot_rootfs_raw_file = os.path.join(this_module.buildroot_images_dir, 'rootfs.ext2')
    this_module.buildroot_qcow2_file = this_module.buildroot_rootfs_raw_file + '.qcow2'
    this_module.staging_dir = os.path.join(this_module.out_dir, 'staging', args.arch)
    this_module.buildroot_staging_dir = os.path.join(this_module.buildroot_build_dir, 'staging')
    this_module.target_dir = os.path.join(this_module.buildroot_build_dir, 'target')
    this_module.run_dir_base = os.path.join(this_module.out_dir, 'run')
    this_module.gem5_run_dir = os.path.join(this_module.run_dir_base, 'gem5', args.arch, str(args.run_id))
    this_module.m5out_dir = os.path.join(this_module.gem5_run_dir, 'm5out')
    this_module.stats_file = os.path.join(this_module.m5out_dir, 'stats.txt')
    this_module.trace_txt_file = os.path.join(this_module.m5out_dir, 'trace.txt')
    this_module.gem5_guest_terminal_file = os.path.join(this_module.m5out_dir, 'system.terminal')
    this_module.gem5_readfile = os.path.join(this_module.gem5_run_dir, 'readfile')
    this_module.gem5_termout_file = os.path.join(this_module.gem5_run_dir, 'termout.txt')
    this_module.qemu_run_dir = os.path.join(this_module.run_dir_base, 'qemu', args.arch, str(args.run_id))
    this_module.qemu_trace_basename = 'trace.bin'
    this_module.qemu_trace_file = os.path.join(this_module.qemu_run_dir, 'trace.bin')
    this_module.qemu_trace_txt_file = os.path.join(this_module.qemu_run_dir, 'trace.txt')
    this_module.qemu_termout_file = os.path.join(this_module.qemu_run_dir, 'termout.txt')
    this_module.qemu_rrfile = os.path.join(this_module.qemu_run_dir, 'rrfile')
    this_module.qemu_guest_terminal_file = os.path.join(this_module.m5out_dir, qemu_termout_file)
    this_module.gem5_out_dir = os.path.join(this_module.out_dir, 'gem5')
    if args.gem5_build_dir is None:
        this_module.gem5_build_dir = os.path.join(this_module.gem5_out_dir, args.gem5_build_id, args.gem5_build_type)
    else:
        this_module.gem5_build_dir = args.gem5_build_dir
    this_module.gem5_fake_iso = os.path.join(this_module.gem5_out_dir, 'fake.iso')
    this_module.gem5_m5term = os.path.join(this_module.gem5_build_dir, 'm5term')
    this_module.gem5_build_build_dir = os.path.join(this_module.gem5_build_dir, 'build')
    this_module.gem5_executable = os.path.join(this_module.gem5_build_build_dir, gem5_arch, 'gem5.{}'.format(args.gem5_build_type))
    this_module.gem5_system_dir = os.path.join(this_module.gem5_build_dir, 'system')
    this_module.crosstool_ng_out_dir = os.path.join(this_module.out_dir, 'crosstool-ng')
    this_module.crosstool_ng_buildid_dir = os.path.join(this_module.crosstool_ng_out_dir, 'build', args.crosstool_ng_build_id)
    this_module.crosstool_ng_install_dir = os.path.join(this_module.crosstool_ng_buildid_dir, 'install', args.arch)
    this_module.crosstool_ng_bin_dir = os.path.join(this_module.crosstool_ng_install_dir, 'bin')
    this_module.crosstool_ng_util_dir = os.path.join(this_module.crosstool_ng_buildid_dir, 'util')
    this_module.crosstool_ng_config = os.path.join(this_module.crosstool_ng_util_dir, '.config')
    this_module.crosstool_ng_defconfig = os.path.join(this_module.crosstool_ng_util_dir, 'defconfig')
    this_module.crosstool_ng_executable = os.path.join(this_module.crosstool_ng_util_dir, 'ct-ng')
    this_module.crosstool_ng_build_dir = os.path.join(this_module.crosstool_ng_buildid_dir, 'build')
    this_module.crosstool_ng_download_dir = os.path.join(this_module.crosstool_ng_out_dir, 'download')
    this_module.gem5_default_src_dir = os.path.join(submodules_dir, 'gem5')
    if args.gem5_source_dir is not None:
        this_module.gem5_src_dir = args.gem5_source_dir
        assert(os.path.exists(args.gem5_source_dir))
    else:
        if args.gem5_worktree is not None:
            this_module.gem5_src_dir = os.path.join(this_module.gem5_non_default_src_root_dir, args.gem5_worktree)
        else:
            this_module.gem5_src_dir = this_module.gem5_default_src_dir
    this_module.gem5_m5_src_dir = os.path.join(this_module.gem5_src_dir, 'util', 'm5')
    this_module.gem5_m5_build_dir = os.path.join(this_module.out_dir, 'util', 'm5')
    if args.gem5:
        this_module.executable = this_module.gem5_executable
        this_module.run_dir = this_module.gem5_run_dir
        this_module.termout_file = this_module.gem5_termout_file
        this_module.guest_terminal_file = gem5_guest_terminal_file
    else:
        this_module.executable = this_module.qemu_executable
        this_module.run_dir = this_module.qemu_run_dir
        this_module.termout_file = this_module.qemu_termout_file
        this_module.guest_terminal_file = qemu_guest_terminal_file
    this_module.gem5_config_dir = os.path.join(this_module.gem5_src_dir, 'configs')
    this_module.gem5_se_file = os.path.join(this_module.gem5_config_dir, 'example', 'se.py')
    this_module.gem5_fs_file = os.path.join(this_module.gem5_config_dir, 'example', 'fs.py')
    this_module.run_cmd_file = os.path.join(this_module.run_dir, 'run.sh')

    # Linux
    this_module.linux_buildroot_build_dir = os.path.join(this_module.buildroot_build_build_dir, 'linux-custom')
    this_module.linux_build_dir = os.path.join(this_module.out_dir, 'linux', args.linux_build_id, args.arch)
    this_module.lkmc_vmlinux = os.path.join(this_module.linux_build_dir, "vmlinux")
    if args.arch == 'arm':
        this_module.linux_arch = 'arm'
        this_module.linux_image_prefix = os.path.join('arch', this_module.linux_arch, 'boot', 'zImage')
    elif args.arch == 'aarch64':
        this_module.linux_arch = 'arm64'
        this_module.linux_image_prefix = os.path.join('arch', this_module.linux_arch, 'boot', 'Image')
    elif args.arch == 'x86_64':
        this_module.linux_arch = 'x86'
        this_module.linux_image_prefix = os.path.join('arch', this_module.linux_arch, 'boot', 'bzImage')
    this_module.lkmc_linux_image = os.path.join(this_module.linux_build_dir, this_module.linux_image_prefix)
    this_module.buildroot_linux_image = os.path.join(this_module.buildroot_linux_build_dir, linux_image_prefix)
    if args.buildroot_linux:
        this_module.vmlinux = this_module.buildroot_vmlinux
        this_module.linux_image = this_module.buildroot_linux_image
    else:
        this_module.vmlinux = this_module.lkmc_vmlinux
        this_module.linux_image = this_module.lkmc_linux_image

    # Kernel modules.
    this_module.kernel_modules_build_base_dir = os.path.join(this_module.out_dir, 'kernel_modules')
    this_module.kernel_modules_build_dir = os.path.join(this_module.kernel_modules_build_base_dir, args.arch)
    this_module.kernel_modules_build_subdir = os.path.join(this_module.kernel_modules_build_dir, kernel_modules_subdir)
    this_module.kernel_modules_build_host_dir = os.path.join(this_module.kernel_modules_build_base_dir, 'host')
    this_module.kernel_modules_build_host_subdir = os.path.join(this_module.kernel_modules_build_host_dir, kernel_modules_subdir)
    this_module.userland_build_dir = os.path.join(this_module.out_dir, 'userland', args.userland_build_id, args.arch)
    this_module.out_rootfs_overlay_dir = os.path.join(this_module.out_dir, 'rootfs_overlay', args.arch)
    this_module.out_rootfs_overlay_bin_dir = os.path.join(this_module.out_rootfs_overlay_dir, 'bin')

    # Ports
    if args.port_offset is None:
        try:
            args.port_offset = int(args.run_id)
        except ValueError:
            args.port_offset = 0
    if args.gem5:
        this_module.gem5_telnet_port = 3456 + args.port_offset
        this_module.gdb_port = 7000 + args.port_offset
    else:
        this_module.qemu_base_port = 45454 + 10 * args.port_offset
        this_module.qemu_monitor_port = this_module.qemu_base_port + 0
        this_module.qemu_hostfwd_generic_port = this_module.qemu_base_port + 1
        this_module.qemu_hostfwd_ssh_port = this_module.qemu_base_port + 2
        this_module.qemu_gdb_port = this_module.qemu_base_port + 3
        this_module.extra_serial_port = this_module.qemu_base_port + 4
        this_module.gdb_port = this_module.qemu_gdb_port
        this_module.qemu_background_serial_file = os.path.join(this_module.qemu_run_dir, 'background.log')

    # Baremetal.
    this_module.baremetal = args.baremetal
    this_module.baremetal_lib_basename = 'lib'
    this_module.baremetal_src_dir = os.path.join(this_module.root_dir, 'baremetal')
    this_module.baremetal_src_lib_dir = os.path.join(this_module.baremetal_src_dir, this_module.baremetal_lib_basename)
    if args.gem5:
        this_module.simulator_name = 'gem5'
    else:
        this_module.simulator_name = 'qemu'
    this_module.baremetal_build_dir = os.path.join(out_dir, 'baremetal', args.arch, this_module.simulator_name, this_module.machine)
    this_module.baremetal_build_lib_dir = os.path.join(this_module.baremetal_build_dir, this_module.baremetal_lib_basename)
    this_module.baremetal_build_ext = '.elf'

    # Docker
    this_module.docker_build_dir = os.path.join(this_module.out_dir, 'docker', args.arch)
    this_module.docker_tar_dir = os.path.join(this_module.docker_build_dir, 'export')
    this_module.docker_tar_file = os.path.join(this_module.docker_build_dir, 'export.tar')
    this_module.docker_rootfs_raw_file = os.path.join(this_module.docker_build_dir, 'export.ext2')
    this_module.docker_qcow2_file = os.path.join(this_module.docker_rootfs_raw_file + '.qcow2')
    if args.docker:
        this_module.rootfs_raw_file = this_module.docker_rootfs_raw_file
        this_module.qcow2_file = this_module.docker_qcow2_file
    else:
        this_module.rootfs_raw_file = this_module.buildroot_rootfs_raw_file
        this_module.qcow2_file = this_module.buildroot_qcow2_file

    # Image.
    if args.baremetal is None:
        if args.gem5:
            this_module.image = this_module.vmlinux
            this_module.disk_image = this_module.rootfs_raw_file
        else:
            this_module.image = this_module.linux_image
            this_module.disk_image = this_module.qcow2_file
    else:
        this_module.disk_image = this_module.gem5_fake_iso
        if args.baremetal == 'all':
            path = args.baremetal
        else:
            path = this_module.resolve_executable(
                args.baremetal,
                this_module.baremetal_src_dir,
                this_module.baremetal_build_dir,
                this_module.baremetal_build_ext,
            )
            source_path_noext = os.path.splitext(os.path.join(
                this_module.baremetal_src_dir,
                os.path.relpath(path, this_module.baremetal_build_dir)
            ))[0]
            for ext in [c_ext, asm_ext]:
                source_path = source_path_noext + ext
                if os.path.exists(source_path):
                    this_module.source_path = source_path
                    break
        this_module.image = path
    return args

def resolve_executable(in_path, magic_in_dir, magic_out_dir, out_ext):
    if os.path.isabs(in_path):
        return in_path
    else:
        paths = [
            os.path.join(magic_out_dir, in_path),
            os.path.join(
                magic_out_dir,
                os.path.relpath(in_path, magic_in_dir),
            )
        ]
        paths[:] = [os.path.splitext(path)[0] + out_ext for path in paths]
        for path in paths:
            if os.path.exists(path):
                return path
        raise Exception('Executable file not found. Tried:\n' + '\n'.join(paths))

def resolve_userland(path):
    global this_module
    return this_module.resolve_executable(
        path,
        this_module.userland_src_dir,
        this_module.userland_build_dir,
        this_module.userland_build_ext,
    )

def setup_dry_run_arguments(args):
    this_module.dry_run = args.dry_run

def shlex_split(string):
    '''
    shlex_split, but also add Newline after every word.

    Not perfect since it does not group arguments, but I don't see a solution.
    '''
    return this_module.add_newlines(shlex.split(string))

def strip_newlines(cmd):
    return [x for x in cmd if x != this_module.Newline]

def write_configs(config_path, configs, config_fragments=None):
    """
    Write extra configs into the Buildroot config file.
    TODO Can't get rid of these for now with nice fragments:
    http://stackoverflow.com/questions/44078245/is-it-possible-to-use-config-fragments-with-buildroots-config
    """
    global this_module
    if config_fragments is None:
        config_fragments = []
    with open(config_path, 'a') as config_file:
        for config_fragment in config_fragments:
            with open(config_fragment, 'r') as config_fragment_file:
                print_cmd(['cat', config_fragment, '>>', config_path])
                if not this_module.dry_run:
                    for line in config_fragment_file:
                        config_file.write(line)
    write_string_to_file(config_path, '\n'.join(configs), mode='a')
