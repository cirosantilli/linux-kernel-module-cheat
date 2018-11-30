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

import cli_function

common = sys.modules[__name__]

# Fixed parameters that don't depend on CLI arguments.
consts = {}
consts['repo_short_id'] = 'lkmc'
# https://stackoverflow.com/questions/20010199/how-to-determine-if-a-process-runs-inside-lxc-docker
consts['in_docker'] = os.path.exists('/.dockerenv')
consts['root_dir'] = os.path.dirname(os.path.abspath(__file__))
consts['data_dir'] = os.path.join(consts['root_dir'], 'data')
consts['p9_dir'] = os.path.join(consts['data_dir'], '9p')
consts['gem5_non_default_src_root_dir'] = os.path.join(consts['data_dir'], 'gem5')
if consts['in_docker']:
    consts['out_dir'] = os.path.join(consts['root_dir'], 'out.docker')
else:
    consts['out_dir'] = os.path.join(consts['root_dir'], 'out')
consts['bench_boot'] = os.path.join(consts['out_dir'], 'bench-boot.txt')
consts['packages_dir'] = os.path.join(consts['root_dir'], 'buildroot_packages')
consts['kernel_modules_subdir'] = 'kernel_modules'
consts['kernel_modules_src_dir'] = os.path.join(consts['root_dir'], consts['kernel_modules_subdir'])
consts['userland_subdir'] = 'userland'
consts['userland_src_dir'] = os.path.join(consts['root_dir'], consts['userland_subdir'])
consts['userland_build_ext'] = '.out'
consts['include_subdir'] = 'include'
consts['include_src_dir'] = os.path.join(consts['root_dir'], consts['include_subdir'])
consts['submodules_dir'] = os.path.join(consts['root_dir'], 'submodules')
consts['buildroot_src_dir'] = os.path.join(consts['submodules_dir'], 'buildroot')
consts['crosstool_ng_src_dir'] = os.path.join(consts['submodules_dir'], 'crosstool-ng')
consts['crosstool_ng_supported_archs'] = set(['arm', 'aarch64'])
consts['linux_src_dir'] = os.path.join(consts['submodules_dir'], 'linux')
consts['linux_config_dir'] = os.path.join(consts['root_dir'], 'linux_config')
consts['rootfs_overlay_dir'] = os.path.join(consts['root_dir'], 'rootfs_overlay')
consts['extract_vmlinux'] = os.path.join(consts['linux_src_dir'], 'scripts', 'extract-vmlinux')
consts['qemu_src_dir'] = os.path.join(consts['submodules_dir'], 'qemu')
consts['parsec_benchmark_src_dir'] = os.path.join(consts['submodules_dir'], 'parsec-benchmark')
consts['ccache_dir'] = os.path.join('/usr', 'lib', 'ccache')
consts['default_build_id'] = 'default'
consts['arch_short_to_long_dict'] = collections.OrderedDict([
    ('x', 'x86_64'),
    ('a', 'arm'),
    ('A', 'aarch64'),
])
consts['all_archs'] = [consts['arch_short_to_long_dict'][k] for k in consts['arch_short_to_long_dict']]
consts['arch_choices'] = []
for key in consts['arch_short_to_long_dict']:
    consts['arch_choices'].append(key)
    consts['arch_choices'].append(consts['arch_short_to_long_dict'][key])
consts['default_arch'] = 'x86_64'
consts['gem5_cpt_prefix'] = '^cpt\.'
consts['sha'] = subprocess.check_output(['git', '-C', consts['root_dir'], 'log', '-1', '--format=%H']).decode().rstrip()
consts['release_dir'] = os.path.join(consts['out_dir'], 'release')
consts['release_zip_file'] = os.path.join(consts['release_dir'], 'lkmc-{}.zip'.format(consts['sha']))
consts['github_repo_id'] = 'cirosantilli/linux-kernel-module-cheat'
consts['asm_ext'] = '.S'
consts['c_ext'] = '.c'
consts['header_ext'] = '.h'
consts['kernel_module_ext'] = '.ko'
consts['obj_ext'] = '.o'
consts['config_file'] = os.path.join(consts['data_dir'], 'config.py')
consts['command_prefix'] = '+ '
consts['magic_fail_string'] = b'lkmc_test_fail'

class LkmcCliFunction(cli_function.CliFunction):
    '''
    Common functionality shared across our CLI functions:

    * command timing
    * some common flags, e.g.: --arch, --dry-run, --verbose
    '''
    def __init__(self):
        super().__init__(config_file=common.consts['config_file'])
        self.add_argument(
            '-a', '--arch', choices=common.arch_choices, default=common.default_arch,
            help='CPU architecture. Default: %(default)s'
        )
        self.add_argument(
            '--dry-run',
            default=False,
            help='''\
Print the commands that would be run, but don't run them.

We aim display every command that modifies the filesystem state, and generate
Bash equivalents even for actions taken directly in Python without shelling out.

mkdir are generally omitted since those are obvious
'''
        )
        self.add_argument(
            '-v', '--verbose', default=False, action='store_true',
            help='Show full compilation commands when they are not shown by default.'
        )

    def main(self, **kwargs):
        '''
        Time the main of the derived class.
        '''
        if not kwargs['dry_run']:
            start_time = time.time()
        self.timed_main(**kwargs)
        if not kwargs['dry_run']:
            end_time = time.time()
            common.print_time(end_time - start_time)

    def timed_main(self, **kwargs):
        raise NotImplementedError()

class BuildCliFunction(LkmcCliFunction):
    '''
    A CLI function with common facilities to build stuff, e.g.:

    * `--clean` to clean the build directory
    * `--nproc` to set he number of build threads
    '''
    def __init__(self):
        super().__init__()
        self.add_argument(
            '--clean',
            default=False,
            help='Clean the build instead of building.',
        ),
        self.add_argument(
            '-j',
            '--nproc',
            default=multiprocessing.cpu_count(),
            type=int,
            help='Number of processors to use for the build. Default: use all cores.',
        )

    def clean(self, **kwargs):
        build_dir = self.get_build_dir(kwargs)
        if build_dir is not None:
            common.rmrf(build_dir)

    def build(self, **kwargs):
        '''
        Do the actual main build work.
        '''
        raise NotImplementedError()

    def get_build_dir(self, **kwargs):
        return None

    def timed_main(self, **kwargs):
        '''
        Parse CLI, and to the build based on it.

        The actual build work is done by do_build in implementing classes.
        '''
        if kwargs['clean']:
            self.clean(kwargs)
        else:
            self.build(**kwargs)

class Newline:
    '''
    Singleton class. Can be used in print_cmd to print out nicer command lines
    with --key on the same line as "--key value".
    '''
    pass

def add_newlines(cmd):
    out = []
    for arg in cmd:
        out.extend([arg, common.Newline])
    return out

def assert_crosstool_ng_supports_arch(arch):
    if arch not in common.crosstool_ng_supported_archs:
        raise Exception('arch not yet supported: ' + arch)

def base64_encode(string):
    return base64.b64encode(string.encode()).decode()

def write_string_to_file(path, string, mode='w'):
    if mode == 'a':
        redirect = '>>'
    else:
        redirect = '>'
    print_cmd("cat << 'EOF' {} {}\n{}\nEOF".format(redirect, path, string))
    if not common.dry_run:
        with open(path, 'a') as f:
            f.write(string)

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
        if arg == common.Newline:
            cmd_quote.append(arg)
            newline_count += 1
        else:
            cmd_quote.append(shlex.quote(arg))
    if newline_count > 0:
        cmd_quote = [' '.join(list(y)) for x, y in itertools.groupby(cmd_quote, lambda z: z == common.Newline) if not x]
    out.extend(cmd_quote)
    if newline_count == 1 and cmd[-1] == common.Newline:
        ending = ''
    else:
        ending = last_newline + ';'
    return newline_separator.join(out) + ending

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

def cp(src, dest):
    print_cmd(['cp', src, dest])
    if not common.dry_run:
        shutil.copy2(src, dest)

def gem_list_checkpoint_dirs():
    '''
    List checkpoint directory, oldest first.
    '''
    prefix_re = re.compile(common.gem5_cpt_prefix)
    files = list(filter(lambda x: os.path.isdir(os.path.join(common.m5out_dir, x)) and prefix_re.search(x), os.listdir(common.m5out_dir)))
    files.sort(key=lambda x: os.path.getmtime(os.path.join(common.m5out_dir, x)))
    return files

def get_argparse(default_args=None):
    '''
    Return an argument parser with common arguments set.

    :type default_args: Dict[str,str]
    :type argparse_args: Dict
    '''
    if default_args is None:
        default_args = {}
    if argparse_args is None:
        argparse_args = {}
    emulator_group = parser.add_mutually_exclusive_group(required=False)
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
    emulator_group.add_argument(
        '-g', '--gem5', default=False, action='store_true',
        help='Use gem5 instead of QEMU. Default: False'
    )
    parser.add_argument(
        '--gem5-build-dir',
        help='''\
Use the given directory as the gem5 build directory.
'''
    )
    parser.add_argument(
        '-M', '--gem5-build-id',
        help='''\
gem5 build ID. Allows you to keep multiple separate gem5 builds. Default: {}
'''.format(default_build_id)
    )
    parser.add_argument(
        '--gem5-build-type', default='opt',
        help='gem5 build type, most often used for "debug" builds. Default: %(default)s'
    )
    parser.add_argument(
        '--gem5-source-dir',
        help='''\
Use the given directory as the gem5 source tree. Ignore `--gem5-worktree`.
'''
    )
    parser.add_argument(
        '-N', '--gem5-worktree',
        help='''\
Create and use a git worktree of the gem5 submodule.
See: https://github.com/cirosantilli/linux-kernel-module-cheat#gem5-worktree
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
    emulator_group.add_argument(
        '--qemu', default=False, action='store_true',
        help='''\
Use QEMU as the emulator. This option exists in addition to --gem5
to allow overriding configs from the CLI.
'''
    )
    parser.add_argument(
        '-Q', '--qemu-build-id', default=default_build_id,
        help='QEMU build ID. Allows you to keep multiple separate QEMU builds. Default: %(default)s'
    )
    parser.add_argument(
        '--userland-build-id', default=None
    )

def get_elf_entry(elf_file_path):
    readelf_header = subprocess.check_output([
        common.get_toolchain_tool('readelf'),
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
    if stat_re is None:
        stat_re = '^system.cpu[0-9]*.numCycles$'
    if stats_file is None:
        stats_file = common.stats_file
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
    buildroot_full_prefix = os.path.join(common.host_bin_dir, common.buildroot_toolchain_prefix)
    buildroot_exists = os.path.exists('{}-{}'.format(buildroot_full_prefix, tool))
    crosstool_ng_full_prefix = os.path.join(common.crosstool_ng_bin_dir, common.crosstool_ng_toolchain_prefix)
    crosstool_ng_exists = os.path.exists('{}-{}'.format(crosstool_ng_full_prefix, tool))
    host_tool = '{}-{}'.format(common.ubuntu_toolchain_prefix, tool)
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
        if common['baremetal'] is None:
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
    return '{}-{}'.format(common.get_toolchain_prefix(tool, allowed_toolchains), tool)

def github_make_request(
        authenticate=False,
        data=None,
        extra_headers=None,
        path='',
        subdomain='api',
        url_params=None,
        **extra_request_args
    ):
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
    os.makedirs(common.buildroot_build_build_dir, exist_ok=True)
    os.makedirs(common.gem5_build_dir, exist_ok=True)
    os.makedirs(common.out_rootfs_overlay_dir, exist_ok=True)

def make_run_dirs():
    '''
    Make directories required for the run.
    The user could nuke those anytime between runs to try and clean things up.
    '''
    os.makedirs(common.gem5_run_dir, exist_ok=True)
    os.makedirs(common.p9_dir, exist_ok=True)
    os.makedirs(common.qemu_run_dir, exist_ok=True)

def need_rebuild(srcs, dst):
    if not os.path.exists(dst):
        return True
    for src in srcs:
        if os.path.getmtime(src) > os.path.getmtime(dst):
            return True
    return False

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
    print(common.command_prefix + cmd_string)
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
    if prebuilt or not os.path.exists(common.qemu_img_executable):
        disable_trace = []
        qemu_img_executable = common.qemu_img_basename
    else:
        # Prevent qemu-img from generating trace files like QEMU. Disgusting.
        disable_trace = ['-T', 'pr_manager_run,file=/dev/null', common.Newline,]
        qemu_img_executable = common.qemu_img_executable
    infmt = 'raw'
    outfmt = 'qcow2'
    infile = common.rootfs_raw_file
    outfile = common.qcow2_file
    if reverse:
        tmp = infmt
        infmt = outfmt
        outfmt = tmp
        tmp = infile
        infile = outfile
        outfile = tmp
    common.run_cmd(
        [
            qemu_img_executable, common.Newline,
        ] +
        disable_trace +
        [
            'convert', common.Newline,
            '-f', infmt, common.Newline,
            '-O', outfmt, common.Newline,
            infile, common.Newline,
            outfile, common.Newline,
        ]
    )

def resolve_args(defaults, args, extra_args):
    if extra_args is None:
        extra_args = {}
    argcopy = copy.copy(args)
    argcopy.__dict__ = dict(list(defaults.items()) + list(argcopy.__dict__.items()) + list(extra_args.items()))
    return argcopy

def rmrf(path):
    print_cmd(['rm', '-r', '-f', path])
    if not common.dry_run and os.path.exists(path):
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

    cmd = common.strip_newlines(cmd)
    if not dry_run and not common.dry_run:
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

def calculate_kwargs(**kwargs):
    '''
    Update the kwargs from the command line with derived arguments.
    '''
    if args.qemu or not args.gem5:
        common['emulator'] = 'qemu'
    else:
        common['emulator'] = 'gem5'
    if args.arch in common.arch_short_to_long_dict:
        args.arch = common.arch_short_to_long_dict[args.arch]
    if args.gem5_build_id is None:
        args.gem5_build_id = default_build_id
        gem5_build_id_given = False
    else:
        gem5_build_id_given = True
    if args.userland_build_id is None:
        args.userland_build_id = default_build_id
        common.userland_build_id_given = False
    else:
        common.userland_build_id_given = True
    if args.gem5_worktree is not None and not gem5_build_id_given:
        args.gem5_build_id = args.gem5_worktree
    common['machine'] = args.machine
    common.setup_dry_run_arguments(args)
    common.is_arm = False
    if args.arch == 'arm':
        common['armv'] = 7
        common.gem5_arch = 'ARM'
        common['mcpu'] = 'cortex-a15'
        common.buildroot_toolchain_prefix = 'arm-buildroot-linux-uclibcgnueabihf'
        common.crosstool_ng_toolchain_prefix = 'arm-unknown-eabi'
        common.ubuntu_toolchain_prefix = 'arm-linux-gnueabihf'
        if common['emulator'] == 'gem5':
            if common['machine'] is None:
                common['machine'] = 'VExpress_GEM5_V1'
        else:
            if common['machine'] is None:
                common['machine'] = 'virt'
        common.is_arm = True
    elif args.arch == 'aarch64':
        common['armv'] = 8
        common.gem5_arch = 'ARM'
        common['mcpu'] = 'cortex-a57'
        common.buildroot_toolchain_prefix = 'aarch64-buildroot-linux-uclibc'
        common.crosstool_ng_toolchain_prefix = 'aarch64-unknown-elf'
        common.ubuntu_toolchain_prefix = 'aarch64-linux-gnu'
        if common['emulator'] == 'gem5':
            if common['machine'] is None:
                common['machine'] = 'VExpress_GEM5_V1'
        else:
            if common['machine'] is None:
                common['machine'] = 'virt'
        common.is_arm = True
    elif args.arch == 'x86_64':
        common.crosstool_ng_toolchain_prefix = 'x86_64-unknown-elf'
        common.gem5_arch = 'X86'
        common.buildroot_toolchain_prefix = 'x86_64-buildroot-linux-uclibc'
        common.ubuntu_toolchain_prefix = 'x86_64-linux-gnu'
        if common['emulator'] == 'gem5':
            if common['machine'] is None:
                common['machine'] = 'TODO'
        else:
            if common['machine'] is None:
                common['machine'] = 'pc'
    common.buildroot_out_dir = os.path.join(consts['out_dir'], 'buildroot')
    common.buildroot_build_dir = os.path.join(common.buildroot_out_dir, 'build', args.buildroot_build_id, args.arch)
    common.buildroot_download_dir = os.path.join(common.buildroot_out_dir, 'download')
    common.buildroot_config_file = os.path.join(common.buildroot_build_dir, '.config')
    common.buildroot_build_build_dir = os.path.join(common.buildroot_build_dir, 'build')
    common.buildroot_linux_build_dir = os.path.join(common.buildroot_build_build_dir, 'linux-custom')
    common.buildroot_vmlinux = os.path.join(common.buildroot_linux_build_dir, "vmlinux")
    common.qemu_build_dir = os.path.join(consts['out_dir'], 'qemu', args.qemu_build_id)
    common.qemu_executable_basename = 'qemu-system-{}'.format(args.arch)
    common.qemu_executable = os.path.join(common.qemu_build_dir, '{}-softmmu'.format(args.arch), common.qemu_executable_basename)
    common.qemu_img_basename = 'qemu-img'
    common.qemu_img_executable = os.path.join(common.qemu_build_dir, common.qemu_img_basename)
    common.host_dir = os.path.join(common.buildroot_build_dir, 'host')
    common.host_bin_dir = os.path.join(common.host_dir, 'usr', 'bin')
    common.buildroot_pkg_config = os.path.join(common.host_bin_dir, 'pkg-config')
    common.buildroot_images_dir = os.path.join(common.buildroot_build_dir, 'images')
    common.buildroot_rootfs_raw_file = os.path.join(common.buildroot_images_dir, 'rootfs.ext2')
    common.buildroot_qcow2_file = common.buildroot_rootfs_raw_file + '.qcow2'
    common.staging_dir = os.path.join(consts['out_dir'], 'staging', args.arch)
    common.buildroot_staging_dir = os.path.join(common.buildroot_build_dir, 'staging')
    common.target_dir = os.path.join(common.buildroot_build_dir, 'target')
    common.run_dir_base = os.path.join(consts['out_dir'], 'run')
    common.gem5_run_dir = os.path.join(common.run_dir_base, 'gem5', args.arch, str(args.run_id))
    common.m5out_dir = os.path.join(common.gem5_run_dir, 'm5out')
    common.stats_file = os.path.join(common.m5out_dir, 'stats.txt')
    common.gem5_trace_txt_file = os.path.join(common.m5out_dir, 'trace.txt')
    common.gem5_guest_terminal_file = os.path.join(common.m5out_dir, 'system.terminal')
    common.gem5_readfile = os.path.join(common.gem5_run_dir, 'readfile')
    common.gem5_termout_file = os.path.join(common.gem5_run_dir, 'termout.txt')
    common.qemu_run_dir = os.path.join(common.run_dir_base, 'qemu', args.arch, str(args.run_id))
    common.qemu_trace_basename = 'trace.bin'
    common.qemu_trace_file = os.path.join(common.qemu_run_dir, 'trace.bin')
    common.qemu_trace_txt_file = os.path.join(common.qemu_run_dir, 'trace.txt')
    common.qemu_termout_file = os.path.join(common.qemu_run_dir, 'termout.txt')
    common.qemu_rrfile = os.path.join(common.qemu_run_dir, 'rrfile')
    common.qemu_guest_terminal_file = os.path.join(common.m5out_dir, qemu_termout_file)
    common.gem5_out_dir = os.path.join(consts['out_dir'], 'gem5')
    if args.gem5_build_dir is None:
        common.gem5_build_dir = os.path.join(common.gem5_out_dir, args.gem5_build_id, args.gem5_build_type)
    else:
        common.gem5_build_dir = args.gem5_build_dir
    common.gem5_fake_iso = os.path.join(common.gem5_out_dir, 'fake.iso')
    common.gem5_m5term = os.path.join(common.gem5_build_dir, 'm5term')
    common.gem5_build_build_dir = os.path.join(common.gem5_build_dir, 'build')
    common.gem5_executable = os.path.join(common.gem5_build_build_dir, gem5_arch, 'gem5.{}'.format(args.gem5_build_type))
    common.gem5_system_dir = os.path.join(common.gem5_build_dir, 'system')
    common.crosstool_ng_out_dir = os.path.join(consts['out_dir'], 'crosstool-ng')
    common.crosstool_ng_buildid_dir = os.path.join(common.crosstool_ng_out_dir, 'build', args.crosstool_ng_build_id)
    common.crosstool_ng_install_dir = os.path.join(common.crosstool_ng_buildid_dir, 'install', args.arch)
    common.crosstool_ng_bin_dir = os.path.join(common.crosstool_ng_install_dir, 'bin')
    common.crosstool_ng_util_dir = os.path.join(common.crosstool_ng_buildid_dir, 'util')
    common.crosstool_ng_config = os.path.join(common.crosstool_ng_util_dir, '.config')
    common.crosstool_ng_defconfig = os.path.join(common.crosstool_ng_util_dir, 'defconfig')
    common.crosstool_ng_executable = os.path.join(common.crosstool_ng_util_dir, 'ct-ng')
    common.crosstool_ng_build_dir = os.path.join(common.crosstool_ng_buildid_dir, 'build')
    common.crosstool_ng_download_dir = os.path.join(common.crosstool_ng_out_dir, 'download')
    common.gem5_default_src_dir = os.path.join(submodules_dir, 'gem5')
    if args.gem5_source_dir is not None:
        common.gem5_src_dir = args.gem5_source_dir
        assert(os.path.exists(args.gem5_source_dir))
    else:
        if args.gem5_worktree is not None:
            common.gem5_src_dir = os.path.join(common.gem5_non_default_src_root_dir, args.gem5_worktree)
        else:
            common.gem5_src_dir = common.gem5_default_src_dir
    common.gem5_m5_src_dir = os.path.join(common.gem5_src_dir, 'util', 'm5')
    common.gem5_m5_build_dir = os.path.join(consts['out_dir'], 'util', 'm5')
    if common['emulator'] == 'gem5':
        common['executable'] = common.gem5_executable
        common.run_dir = common.gem5_run_dir
        common.termout_file = common.gem5_termout_file
        common.guest_terminal_file = gem5_guest_terminal_file
        common.trace_txt_file = gem5_trace_txt_file
    else:
        common['executable'] = common.qemu_executable
        common.run_dir = common.qemu_run_dir
        common.termout_file = common.qemu_termout_file
        common.guest_terminal_file = qemu_guest_terminal_file
        common.trace_txt_file = qemu_trace_txt_file
    common.gem5_config_dir = os.path.join(common.gem5_src_dir, 'configs')
    common.gem5_se_file = os.path.join(common.gem5_config_dir, 'example', 'se.py')
    common.gem5_fs_file = os.path.join(common.gem5_config_dir, 'example', 'fs.py')
    common.run_cmd_file = os.path.join(common.run_dir, 'run.sh')

    # Linux
    common.linux_buildroot_build_dir = os.path.join(common.buildroot_build_build_dir, 'linux-custom')
    common.linux_build_dir = os.path.join(consts['out_dir'], 'linux', args.linux_build_id, args.arch)
    common.lkmc_vmlinux = os.path.join(common.linux_build_dir, "vmlinux")
    if args.arch == 'arm':
        common.linux_arch = 'arm'
        common.linux_image_prefix = os.path.join('arch', common.linux_arch, 'boot', 'zImage')
    elif args.arch == 'aarch64':
        common.linux_arch = 'arm64'
        common.linux_image_prefix = os.path.join('arch', common.linux_arch, 'boot', 'Image')
    elif args.arch == 'x86_64':
        common.linux_arch = 'x86'
        common.linux_image_prefix = os.path.join('arch', common.linux_arch, 'boot', 'bzImage')
    common.lkmc_linux_image = os.path.join(common.linux_build_dir, common.linux_image_prefix)
    common.buildroot_linux_image = os.path.join(common.buildroot_linux_build_dir, linux_image_prefix)
    if args.buildroot_linux:
        common['vmlinux'] = common.buildroot_vmlinux
        common.linux_image = common.buildroot_linux_image
    else:
        common['vmlinux'] = common.lkmc_vmlinux
        common.linux_image = common.lkmc_linux_image

    # Kernel modules.
    common.kernel_modules_build_base_dir = os.path.join(consts['out_dir'], 'kernel_modules')
    common.kernel_modules_build_dir = os.path.join(common.kernel_modules_build_base_dir, args.arch)
    common.kernel_modules_build_subdir = os.path.join(common.kernel_modules_build_dir, kernel_modules_subdir)
    common.kernel_modules_build_host_dir = os.path.join(common.kernel_modules_build_base_dir, 'host')
    common.kernel_modules_build_host_subdir = os.path.join(common.kernel_modules_build_host_dir, kernel_modules_subdir)
    common.userland_build_dir = os.path.join(consts['out_dir'], 'userland', args.userland_build_id, args.arch)
    common.out_rootfs_overlay_dir = os.path.join(consts['out_dir'], 'rootfs_overlay', args.arch)
    common.out_rootfs_overlay_bin_dir = os.path.join(common.out_rootfs_overlay_dir, 'bin')

    # Ports
    if args.port_offset is None:
        try:
            args.port_offset = int(args.run_id)
        except ValueError:
            args.port_offset = 0
    if common['emulator'] == 'gem5':
        common.gem5_telnet_port = 3456 + args.port_offset
        common.gdb_port = 7000 + args.port_offset
    else:
        common.qemu_base_port = 45454 + 10 * args.port_offset
        common.qemu_monitor_port = common.qemu_base_port + 0
        common.qemu_hostfwd_generic_port = common.qemu_base_port + 1
        common.qemu_hostfwd_ssh_port = common.qemu_base_port + 2
        common.qemu_gdb_port = common.qemu_base_port + 3
        common.extra_serial_port = common.qemu_base_port + 4
        common.gdb_port = common.qemu_gdb_port
        common.qemu_background_serial_file = os.path.join(common.qemu_run_dir, 'background.log')

    # Baremetal.
    common['baremetal'] = args.baremetal
    common.baremetal_lib_basename = 'lib'
    common.baremetal_src_dir = os.path.join(consts['root_dir'], 'baremetal')
    common.baremetal_src_lib_dir = os.path.join(common.baremetal_src_dir, common.baremetal_lib_basename)
    if common['emulator'] == 'gem5':
        common.simulator_name = 'gem5'
    else:
        common.simulator_name = 'qemu'
    common.baremetal_build_dir = os.path.join(out_dir, 'baremetal', args.arch, common.simulator_name, common['machine'])
    common.baremetal_build_lib_dir = os.path.join(common.baremetal_build_dir, common.baremetal_lib_basename)
    common.baremetal_build_ext = '.elf'

    # Docker
    common.docker_build_dir = os.path.join(consts['out_dir'], 'docker', args.arch)
    common.docker_tar_dir = os.path.join(common.docker_build_dir, 'export')
    common.docker_tar_file = os.path.join(common.docker_build_dir, 'export.tar')
    common.docker_rootfs_raw_file = os.path.join(common.docker_build_dir, 'export.ext2')
    common.docker_qcow2_file = os.path.join(common.docker_rootfs_raw_file + '.qcow2')
    if args.docker:
        common.rootfs_raw_file = common.docker_rootfs_raw_file
        common.qcow2_file = common.docker_qcow2_file
    else:
        common.rootfs_raw_file = common.buildroot_rootfs_raw_file
        common.qcow2_file = common.buildroot_qcow2_file

    # Image.
    if common['baremetal'] is None:
        if common['emulator'] == 'gem5':
            common['image'] = common['vmlinux']
            common.disk_image = common.rootfs_raw_file
        else:
            common['image'] = common.linux_image
            common.disk_image = common.qcow2_file
    else:
        common.disk_image = common.gem5_fake_iso
        if common['baremetal'] == 'all':
            path = common['baremetal']
        else:
            path = common.resolve_executable(
                common['baremetal'],
                common.baremetal_src_dir,
                common.baremetal_build_dir,
                common.baremetal_build_ext,
            )
            source_path_noext = os.path.splitext(os.path.join(
                common.baremetal_src_dir,
                os.path.relpath(path, common.baremetal_build_dir)
            ))[0]
            for ext in [c_ext, asm_ext]:
                source_path = source_path_noext + ext
                if os.path.exists(source_path):
                    common.source_path = source_path
                    break
        common['image'] = path
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
    return common.resolve_executable(
        path,
        common.userland_src_dir,
        common.userland_build_dir,
        common.userland_build_ext,
    )

def setup_dry_run_arguments(args):
    common.dry_run = args.dry_run

def shlex_split(string):
    '''
    shlex_split, but also add Newline after every word.

    Not perfect since it does not group arguments, but I don't see a solution.
    '''
    return common.add_newlines(shlex.split(string))

def strip_newlines(cmd):
    return [x for x in cmd if x != common.Newline]

def write_configs(config_path, configs, config_fragments=None):
    """
    Write extra configs into the Buildroot config file.
    TODO Can't get rid of these for now with nice fragments:
    http://stackoverflow.com/questions/44078245/is-it-possible-to-use-config-fragments-with-buildroots-config
    """
    if config_fragments is None:
        config_fragments = []
    with open(config_path, 'a') as config_file:
        for config_fragment in config_fragments:
            with open(config_fragment, 'r') as config_fragment_file:
                print_cmd(['cat', config_fragment, '>>', config_path])
                if not common.dry_run:
                    for line in config_fragment_file:
                        config_file.write(line)
    write_string_to_file(config_path, '\n'.join(configs), mode='a')
