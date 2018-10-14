#!/usr/bin/env python3

import argparse
import base64
import collections
import copy
import datetime
import distutils.file_util
import glob
import imp
import json
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

this = sys.modules[__name__]
root_dir = os.path.dirname(os.path.abspath(__file__))
data_dir = os.path.join(root_dir, 'data')
p9_dir = os.path.join(data_dir, '9p')
gem5_non_default_src_root_dir = os.path.join(data_dir, 'gem5')
out_dir = os.path.join(root_dir, 'out')
bench_boot = os.path.join(out_dir, 'bench-boot.txt')
packages_dir = os.path.join(root_dir, 'packages')
lkmc_package_src_dir = os.path.join(this.packages_dir, 'lkmc')
kernel_modules_subdir = 'kernel_modules'
kernel_modules_src_dir = os.path.join(this.lkmc_package_src_dir, this.kernel_modules_subdir)
userland_subdir = 'userland'
userland_src_dir = os.path.join(this.lkmc_package_src_dir, this.userland_subdir)
submodules_dir = os.path.join(root_dir, 'submodules')
buildroot_src_dir = os.path.join(submodules_dir, 'buildroot')
crosstool_ng_src_dir = os.path.join(submodules_dir, 'crosstool-ng')
linux_src_dir = os.path.join(submodules_dir, 'linux')
linux_config_dir = os.path.join(this.root_dir, 'kernel_config')
rootfs_overlay_dir = os.path.join(this.root_dir, 'rootfs_overlay')
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
for key in this.arch_short_to_long_dict:
    arch_choices.append(key)
    arch_choices.append(this.arch_short_to_long_dict[key])
default_arch = 'x86_64'
gem5_cpt_prefix = '^cpt\.'
sha = subprocess.check_output(['git', '-C', root_dir, 'log', '-1', '--format=%H']).decode().rstrip()
release_dir = os.path.join(this.out_dir, 'release')
release_zip_file = os.path.join(this.release_dir, 'lkmc-{}.zip'.format(this.sha))
github_repo_id = 'cirosantilli/linux-kernel-module-cheat'
asm_ext = '.S'
c_ext = '.c'
header_ext = '.h'
kernel_module_ext = '.ko'
obj_ext = '.o'
executable_ext = '.out'
config_file = os.path.join(data_dir, 'config')
command_prefix = '+ '
if os.path.exists(config_file):
    config = imp.load_source('config', config_file)
    configs = {x:getattr(config, x) for x in dir(config) if not x.startswith('__')}

def add_build_arguments(parser):
    parser.add_argument(
        '--clean',
        help='Clean the build instead of building.',
        action='store_true',
    )

def base64_encode(string):
    return base64.b64encode(string.encode()).decode()

def copy_dir_if_update_non_recursive(srcdir, destdir, filter_ext=None):
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
    global this
    prefix_re = re.compile(this.gem5_cpt_prefix)
    files = list(filter(lambda x: os.path.isdir(os.path.join(this.m5out_dir, x)) and prefix_re.search(x), os.listdir(this.m5out_dir)))
    files.sort(key=lambda x: os.path.getmtime(os.path.join(this.m5out_dir, x)))
    return files

def get_argparse(default_args=None, argparse_args=None):
    '''
    Return an argument parser with common arguments set.

    :type default_args: Dict[str,str]
    :type argparse_args: Dict
    '''
    global this
    if default_args is None:
        default_args = {}
    if argparse_args is None:
        argparse_args = {}
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter,
        **argparse_args
    )
    parser.add_argument(
        '-a', '--arch', choices=this.arch_choices, default=this.default_arch,
        help='CPU architecture. Default: %(default)s'
    )
    parser.add_argument(
        '--baremetal',
        help='Use Baremetal examples instead of Linux kernel ones'
    )
    parser.add_argument(
        '--buildroot-build-id',
        default=default_build_id,
        help='Buildroot build ID. Allows you to keep multiple separate gem5 builds. Default: %(default)s'
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
        help='gem5 build ID. Allows you to keep multiple separate gem5 builds. Default: %(default)s'.format(default_build_id)
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
the likelihood of compatibility.
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
        '-t', '--gem5-build-type', default='opt',
        help='gem5 build type, most often used for "debug" builds. Default: %(default)s'
    )
    parser.add_argument(
        '-v', '--verbose', default=False, action='store_true',
        help='Show full compilation commands when they are not shown by default.'
    )
    if hasattr(this, 'configs'):
        defaults = this.configs.copy()
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
    global this
    readelf_header = subprocess.check_output([
        this.get_toolchain_tool('readelf'),
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
    global this
    if stat_re is None:
        stat_re = '^system.cpu[0-9]*.numCycles$'
    if stats_file is None:
        stats_file = this.stats_file
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
    buildroot_full_prefix = os.path.join(this.host_bin_dir, this.buildroot_toolchain_prefix)
    buildroot_exists = os.path.exists('{}-{}'.format(buildroot_full_prefix, tool))
    crosstool_ng_full_prefix = os.path.join(this.crosstool_ng_bin_dir, this.crosstool_ng_toolchain_prefix)
    crosstool_ng_exists = os.path.exists('{}-{}'.format(crosstool_ng_full_prefix, tool))
    host_tool = '{}-{}'.format(this.ubuntu_toolchain_prefix, tool)
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
        if this.baremetal is None:
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
    return '{}-{}'.format(this.get_toolchain_prefix(tool, allowed_toolchains), tool)

def github_make_request(
        authenticate=False,
        data=None,
        extra_headers=None,
        path='',
        subdomain='api',
        url_params=None,
        **extra_request_args
    ):
    global this
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
    global this
    os.makedirs(this.build_dir, exist_ok=True)
    os.makedirs(this.gem5_build_dir, exist_ok=True)

def make_run_dirs():
    '''
    Make directories required for the run.
    The user could nuke those anytime between runs to try and clean things up.
    '''
    global this
    os.makedirs(this.gem5_run_dir, exist_ok=True)
    os.makedirs(this.p9_dir, exist_ok=True)
    os.makedirs(this.qemu_run_dir, exist_ok=True)

def print_cmd(cmd, cwd=None, cmd_file=None, extra_env=None, extra_paths=None):
    '''
    Format a command given as a list of strings so that it can
    be viewed nicely and executed by bash directly and print it to stdout.

    Optionally save the command to cmd_file file, and add extra_env
    environment variables to the command generated.
    '''
    newline_separator = ' \\\n'
    out = []
    if extra_env is None:
        extra_env = {}
    if cwd is not None:
        out.append('cd {} &&{}'.format(shlex.quote(cwd), newline_separator))
    if extra_paths is not None:
        out.append('PATH="{}:${{PATH}}"'.format(':'.join(extra_paths)) + newline_separator)
    for key in extra_env:
        out.append('{}={}'.format(shlex.quote(key), shlex.quote(extra_env[key])) + newline_separator)
    for arg in cmd:
        out.append(shlex.quote(arg) + newline_separator)
    out = '  '.join(out) + ';\n'
    print(this.command_prefix + out, end='')
    if cmd_file is not None:
        with open(cmd_file, 'w') as f:
            f.write('#!/usr/bin/env bash\n')
            f.write(out)
        st = os.stat(cmd_file)
        os.chmod(cmd_file, st.st_mode | stat.S_IXUSR)

def print_time(ellapsed_seconds):
    hours, rem = divmod(ellapsed_seconds, 3600)
    minutes, seconds = divmod(rem, 60)
    print("time {:02}:{:02}:{:02}".format(int(hours), int(minutes), int(seconds)))

def raw_to_qcow2(prebuilt=False, reverse=False):
    global this
    if prebuilt:
        qemu_img_executable = this.qemu_img_basename
    else:
        qemu_img_executable = this.qemu_img_executable
    infmt = 'raw'
    outfmt = 'qcow2'
    infile = this.rootfs_raw_file
    outfile = this.qcow2_file
    if reverse:
        tmp = infmt
        infmt = outfmt
        outfmt = tmp
        tmp = infile
        infile = outfile
        outfile = tmp
    assert this.run_cmd([
        qemu_img_executable,
        # Prevent qemu-img from generating trace files like QEMU. Disgusting.
        '-T', 'pr_manager_run,file=/dev/null',
        'convert',
        '-f', infmt,
        '-O', outfmt,
        infile,
        outfile,
    ]) == 0

def raise_no_x86(arch):
    if (arch == 'x86_64'):
        raise Exception('x86_64 not yet supported')

def resolve_args(defaults, args, extra_args):
    if extra_args is None:
        extra_args = {}
    argcopy = copy.copy(args)
    argcopy.__dict__ = dict(list(defaults.items()) + list(argcopy.__dict__.items()) + list(extra_args.items()))
    return argcopy

def rmrf(path):
    if os.path.exists(path):
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
        **kwargs
    ):
    '''
    Run a command. Write the command to stdout before running it.

    Wait until the command finishes execution.

    :param cmd: command to run
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
    # But if I do this, my terminal gets broken at the end. Why, why, why.
    # https://stackoverflow.com/questions/14207708/ioerror-errno-32-broken-pipe-python
    # Ignoring the exception is not enough as it prints a warning anyways.
    #sigpipe_old = signal.getsignal(signal.SIGPIPE)
    #signal.signal(signal.SIGPIPE, signal.SIG_DFL)

    if not dry_run:
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
        return proc.returncode
    else:
        return 0

def setup(parser):
    '''
    Parse the command line arguments, and setup several variables based on them.
    Typically done after getting inputs from the command line arguments.
    '''
    global this
    args = parser.parse_args()
    if args.arch in this.arch_short_to_long_dict:
        args.arch = this.arch_short_to_long_dict[args.arch]
    if args.gem5_build_id is None:
        args.gem5_build_id = default_build_id
        gem5_build_id_given = False
    else:
        gem5_build_id_given = True
    if args.gem5_worktree is not None and not gem5_build_id_given:
        args.gem5_build_id = args.gem5_worktree
    this.machine = args.machine
    if args.arch == 'arm':
        this.armv = 7
        this.gem5_arch = 'ARM'
        this.mcpu = 'cortex-a15'
        this.buildroot_toolchain_prefix = 'arm-buildroot-linux-uclibcgnueabihf'
        this.crosstool_ng_toolchain_prefix = 'arm-unknown-eabi'
        this.ubuntu_toolchain_prefix = 'arm-linux-gnueabihf'
        if args.gem5:
            if this.machine is None:
                this.machine = 'VExpress_GEM5_V1'
        else:
            if this.machine is None:
                this.machine = 'virt'
    elif args.arch == 'aarch64':
        this.armv = 8
        this.gem5_arch = 'ARM'
        this.mcpu = 'cortex-a57'
        this.buildroot_toolchain_prefix = 'aarch64-buildroot-linux-uclibc'
        this.crosstool_ng_toolchain_prefix = 'aarch64-unknown-elf'
        this.ubuntu_toolchain_prefix = 'aarch64-linux-gnu'
        if args.gem5:
            if this.machine is None:
                this.machine = 'VExpress_GEM5_V1'
        else:
            if this.machine is None:
                this.machine = 'virt'
    elif args.arch == 'x86_64':
        this.crosstool_ng_toolchain_prefix = 'x86_64-unknown-elf'
        this.gem5_arch = 'X86'
        this.buildroot_toolchain_prefix = 'x86_64-buildroot-linux-uclibc'
        this.ubuntu_toolchain_prefix = 'x86_64-linux-gnu'
        if args.gem5:
            if this.machine is None:
                this.machine = 'TODO'
        else:
            if this.machine is None:
                this.machine = 'pc'
    this.buildroot_out_dir = os.path.join(this.out_dir, 'buildroot')
    this.buildroot_build_dir = os.path.join(this.buildroot_out_dir, 'build', args.buildroot_build_id, args.arch)
    this.buildroot_download_dir = os.path.join(this.buildroot_out_dir, 'download')
    this.buildroot_config_file = os.path.join(this.buildroot_build_dir, '.config')
    this.build_dir = os.path.join(this.buildroot_build_dir, 'build')
    this.qemu_build_dir = os.path.join(this.out_dir, 'qemu', args.qemu_build_id)
    this.qemu_executable_basename = 'qemu-system-{}'.format(args.arch)
    this.qemu_executable = os.path.join(this.qemu_build_dir, '{}-softmmu'.format(args.arch), this.qemu_executable_basename)
    this.qemu_img_basename = 'qemu-img'
    this.qemu_img_executable = os.path.join(this.qemu_build_dir, this.qemu_img_basename)
    this.qemu_guest_build_dir = os.path.join(this.build_dir, 'qemu-custom')
    this.host_dir = os.path.join(this.buildroot_build_dir, 'host')
    this.host_bin_dir = os.path.join(this.host_dir, 'usr', 'bin')
    this.buildroot_pkg_config = os.path.join(this.host_bin_dir, 'pkg-config')
    this.buildroot_images_dir = os.path.join(this.buildroot_build_dir, 'images')
    this.buildroot_rootfs_raw_file = os.path.join(this.buildroot_images_dir, 'rootfs.ext2')
    this.buildroot_qcow2_file = this.buildroot_rootfs_raw_file + '.qcow2'
    this.staging_dir = os.path.join(this.out_dir, 'staging', args.arch)
    this.buildroot_staging_dir = os.path.join(this.buildroot_build_dir, 'staging')
    this.target_dir = os.path.join(this.buildroot_build_dir, 'target')
    this.run_dir_base = os.path.join(this.out_dir, 'run')
    this.gem5_run_dir = os.path.join(this.run_dir_base, 'gem5', args.arch, str(args.run_id))
    this.m5out_dir = os.path.join(this.gem5_run_dir, 'm5out')
    this.stats_file = os.path.join(this.m5out_dir, 'stats.txt')
    this.trace_txt_file = os.path.join(this.m5out_dir, 'trace.txt')
    this.gem5_readfile = os.path.join(this.gem5_run_dir, 'readfile')
    this.gem5_termout_file = os.path.join(this.gem5_run_dir, 'termout.txt')
    this.qemu_run_dir = os.path.join(this.run_dir_base, 'qemu', args.arch, str(args.run_id))
    this.qemu_trace_basename = 'trace.bin'
    this.qemu_trace_file = os.path.join(this.qemu_run_dir, 'trace.bin')
    this.qemu_trace_txt_file = os.path.join(this.qemu_run_dir, 'trace.txt')
    this.qemu_termout_file = os.path.join(this.qemu_run_dir, 'termout.txt')
    this.qemu_rrfile = os.path.join(this.qemu_run_dir, 'rrfile')
    this.gem5_out_dir = os.path.join(this.out_dir, 'gem5')
    if args.gem5_build_dir is None:
        this.gem5_build_dir = os.path.join(this.gem5_out_dir, args.gem5_build_id, args.gem5_build_type)
    else:
        this.gem5_build_dir = args.gem5_build_dir
    this.gem5_fake_iso = os.path.join(this.gem5_out_dir, 'fake.iso')
    this.gem5_m5term = os.path.join(this.gem5_build_dir, 'm5term')
    this.gem5_build_build_dir = os.path.join(this.gem5_build_dir, 'build')
    this.gem5_executable = os.path.join(this.gem5_build_build_dir, gem5_arch, 'gem5.{}'.format(args.gem5_build_type))
    this.gem5_system_dir = os.path.join(this.gem5_build_dir, 'system')
    this.crosstool_ng_out_dir = os.path.join(this.out_dir, 'crosstool-ng')
    this.crosstool_ng_buildid_dir = os.path.join(this.crosstool_ng_out_dir, 'build', args.crosstool_ng_build_id)
    this.crosstool_ng_install_dir = os.path.join(this.crosstool_ng_buildid_dir, 'install', args.arch)
    this.crosstool_ng_bin_dir = os.path.join(this.crosstool_ng_install_dir, 'bin')
    this.crosstool_ng_util_dir = os.path.join(this.crosstool_ng_buildid_dir, 'util')
    this.crosstool_ng_config = os.path.join(this.crosstool_ng_util_dir, '.config')
    this.crosstool_ng_defconfig = os.path.join(this.crosstool_ng_util_dir, 'defconfig')
    this.crosstool_ng_executable = os.path.join(this.crosstool_ng_util_dir, 'ct-ng')
    this.crosstool_ng_build_dir = os.path.join(this.crosstool_ng_buildid_dir, 'build')
    this.crosstool_ng_download_dir = os.path.join(this.crosstool_ng_out_dir, 'download')
    this.gem5_default_src_dir = os.path.join(submodules_dir, 'gem5')
    if args.gem5_source_dir is not None:
        this.gem5_src_dir = args.gem5_source_dir
        assert(os.path.exists(args.gem5_source_dir))
    else:
        if args.gem5_worktree is not None:
            this.gem5_src_dir = os.path.join(this.gem5_non_default_src_root_dir, args.gem5_worktree)
        else:
            this.gem5_src_dir = this.gem5_default_src_dir
    if args.gem5:
        this.executable = this.gem5_executable
        this.run_dir = this.gem5_run_dir
        this.termout_file = this.gem5_termout_file
    else:
        this.executable = this.qemu_executable
        this.run_dir = this.qemu_run_dir
        this.termout_file = this.qemu_termout_file
    this.gem5_config_dir = os.path.join(this.gem5_src_dir, 'configs')
    this.gem5_se_file = os.path.join(this.gem5_config_dir, 'example', 'se.py')
    this.gem5_fs_file = os.path.join(this.gem5_config_dir, 'example', 'fs.py')
    this.run_cmd_file = os.path.join(this.run_dir, 'run.sh')

    # Linux
    this.linux_buildroot_build_dir = os.path.join(this.build_dir, 'linux-custom')
    this.linux_build_dir = os.path.join(this.out_dir, 'linux', args.linux_build_id, args.arch)
    this.vmlinux = os.path.join(this.linux_build_dir, "vmlinux")
    if args.arch == 'arm':
        this.linux_arch = 'arm'
        this.linux_image = os.path.join('arch', this.linux_arch, 'boot', 'zImage')
    elif args.arch == 'aarch64':
        this.linux_arch = 'arm64'
        this.linux_image = os.path.join('arch', this.linux_arch, 'boot', 'Image')
    elif args.arch == 'x86_64':
        this.linux_arch = 'x86'
        this.linux_image = os.path.join('arch', this.linux_arch, 'boot', 'bzImage')
    this.linux_image = os.path.join(this.linux_build_dir, linux_image)

    # Kernel modules.
    this.kernel_modules_build_base_dir = os.path.join(this.out_dir, 'kernel_modules')
    this.kernel_modules_build_dir = os.path.join(this.kernel_modules_build_base_dir, args.arch)
    this.kernel_modules_build_host_dir = os.path.join(this.kernel_modules_build_base_dir, 'host')
    this.userland_build_dir = os.path.join(this.out_dir, 'userland', args.arch)
    this.out_rootfs_overlay_dir = os.path.join(this.out_dir, 'rootfs_overlay', args.arch)

    # Ports
    if args.port_offset is None:
        try:
            args.port_offset = int(args.run_id)
        except ValueError:
            args.port_offset = 0
    if args.gem5:
        this.gem5_telnet_port = 3456 + args.port_offset
        this.gdb_port = 7000 + args.port_offset
    else:
        this.qemu_base_port = 45454 + 10 * args.port_offset
        this.qemu_monitor_port = this.qemu_base_port + 0
        this.qemu_hostfwd_generic_port = this.qemu_base_port + 1
        this.qemu_hostfwd_ssh_port = this.qemu_base_port + 2
        this.qemu_gdb_port = this.qemu_base_port + 3
        this.gdb_port = this.qemu_gdb_port

    # Baremetal.
    this.baremetal = args.baremetal
    this.baremetal_lib_basename = 'lib'
    this.baremetal_src_dir = os.path.join(this.root_dir, 'baremetal')
    this.baremetal_src_lib_dir = os.path.join(this.baremetal_src_dir, this.baremetal_lib_basename)
    if args.gem5:
        this.simulator_name = 'gem5'
    else:
        this.simulator_name = 'qemu'
    this.baremetal_out_dir = os.path.join(out_dir, 'baremetal', args.arch, this.simulator_name, this.machine)
    this.baremetal_out_lib_dir = os.path.join(this.baremetal_out_dir, this.baremetal_lib_basename)
    this.baremetal_out_ext = '.elf'


    # Docker
    this.docker_build_dir = os.path.join(this.out_dir, 'docker', args.arch)
    this.docker_tar_dir = os.path.join(this.docker_build_dir, 'export')
    this.docker_tar_file = os.path.join(this.docker_build_dir, 'export.tar')
    this.docker_rootfs_raw_file = os.path.join(this.docker_build_dir, 'export.ext2')
    this.docker_qcow2_file = os.path.join(this.docker_rootfs_raw_file + '.qcow2')
    if args.docker:
        this.rootfs_raw_file = this.docker_rootfs_raw_file
        this.qcow2_file = this.docker_qcow2_file
    else:
        this.rootfs_raw_file = this.buildroot_rootfs_raw_file
        this.qcow2_file = this.buildroot_qcow2_file

    # Image.
    if args.baremetal is None:
        if args.gem5:
            this.image = this.vmlinux
            this.disk_image = this.rootfs_raw_file
        else:
            this.image = this.linux_image
            this.disk_image = this.qcow2_file
    else:
        this.disk_image = this.gem5_fake_iso
        paths = [
            os.path.join(this.baremetal_out_dir, this.baremetal),
            os.path.join(
                this.baremetal_out_dir,
                os.path.relpath(this.baremetal, this.baremetal_src_dir),
            )
        ]
        paths[:] = [os.path.splitext(path)[0] + this.baremetal_out_ext for path in paths]
        found = False
        for path in paths:
            if os.path.exists(path):
                found = True
                break
        if not found and this.baremetal != 'all':
            raise Exception('Baremetal ELF file not found. Tried:\n' + '\n'.join(paths))
        this.image = path
    return args

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
            with open(config_fragment, 'r') as config_fragment:
                for line in config_fragment:
                    config_file.write(line)
        for config in configs:
            config_file.write(config + '\n')
