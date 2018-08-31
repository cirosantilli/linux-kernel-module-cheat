#!/usr/bin/env python3

import argparse
import base64
import glob
import imp
import os
import re
import shlex
import signal
import stat
import subprocess
import sys

this = sys.modules[__name__]

# TODO
## Benchmark a command.
##
## $1: command to benchmark
## $2: where to append write results to. Default: /dev/null.
##
## Result format:
##
## cmd <command run>
## time <time in seconds to finish>
## exit_status <exit status>
#bench_cmd() (
#    cmd = "$1"
#    results_file = "${2:-/dev/null}"
#    printf 'cmd ' >> "$results_file"
#    env time --append -f 'time %e' --output = "$results_file" "${root_dir}/eeval" -a "$cmd" "$results_file"
#    printf "exit_status $?\n" >> "$results_file"
#)

def base64_encode(string):
    return base64.b64encode(string.encode()).decode()

def error(msg):
    print('error: {}'.format(msg), file=sys.stderr)
    sys.exit(1)

def gem_list_checkpoint_dirs():
    """
    List checkpoint directory, oldest first.
    """
    global this
    prefix_re = re.compile(this.gem5_cpt_prefix)
    files = list(filter(lambda x: os.path.isdir(os.path.join(this.m5out_dir, x)) and prefix_re.search(x), os.listdir(this.m5out_dir)))
    files.sort(key=lambda x: os.path.getmtime(os.path.join(this.m5out_dir, x)))
    return files

def get_argparse(default_args=None, argparse_args=None):
    """
    Return an argument parser with common arguments set.
    """
    global this
    if default_args is None:
        default_args = {}
    if argparse_args is None:
        argparse_args = {}
    arch_choices = []
    for key in this.arch_map:
        arch_choices.append(key)
        arch_choices.append(this.arch_map[key])
    default_build_id = 'default'
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter,
        **argparse_args
    )
    parser.add_argument(
        '-a', '--arch', choices=arch_choices, default='x86_64',
        help='CPU architecture. Default: %(default)s'
    )
    parser.add_argument(
        '-g', '--gem5', default=False, action='store_true',
        help='Use gem5 instead of QEMU'
    )
    parser.add_argument(
        '-L', '--linux-build-id', default=default_build_id,
        help='Linux build ID. Allows you to keep multiple separate Linux builds. Default: %(default)s'
    )
    parser.add_argument(
        '-M', '--gem5-build-id', default=default_build_id,
        help='gem5 build ID. Allows you to keep multiple separate gem5 builds. Default: %(default)s'
    )
    parser.add_argument(
        '-N', '--gem5-worktree',
        help="""\
gem5 git worktree to use for build and Python scripts at runtime. Automatically
create a new git worktree with the given id if one does not exist. If not
given, just use the submodule source.
"""
    )
    parser.add_argument(
        '-n', '--run-id', default='0',
        help="""\
ID for run outputs such as gem5's m5out. Allows you to do multiple runs,
and then inspect separate outputs later in different output directories.
Default: %(default)s
"""
    )
    parser.add_argument(
        '--port-offset', type=int,
        help="""\
Increase the ports to be used such as for GDB by an offset to run multiple
instances in parallel.
Default: the run ID (-n) if that is an integer, otherwise 0.
"""
    )
    parser.add_argument(
        '-Q', '--qemu-build-id', default=default_build_id,
        help='QEMU build ID. Allows you to keep multiple separate QEMU builds. Default: %(default)s'
    )
    parser.add_argument(
        '-s', '--suffix',
        help="""\
Add a custom suffix to the build. E.g., doing `./build -s mysuf` puts all
the build output into `out/x86_64-mysuf`. This allows keep multiple builds
around when you checkout between branches.
"""
    )
    parser.add_argument(
        '-t', '--gem5-build-type', default='opt',
        help='gem5 build type, most often used for "debug" builds. Default: %(default)s'
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

def get_toolchain_tool(tool):
    global this
    return glob.glob(os.path.join(this.host_bin_dir, '*-buildroot-*-{}'.format(tool)))[0]

def print_cmd(cmd, cmd_file=None, extra_env=None):
    """
    Format a command given as a list of strings so that it can
    be viewed nicely and executed by bash directly and print it to stdout.

    Optionally save the command to cmd_file file, and add extra_env
    environment variables to the command generated.
    """
    newline_separator = ' \\\n'
    out = []
    for key in extra_env:
        out.extend(['{}={}'.format(shlex.quote(key), shlex.quote(extra_env[key])), newline_separator])
    for arg in cmd:
        out.extend([shlex.quote(arg), newline_separator])
    out = ''.join(out)
    print(out)
    if cmd_file is not None:
        with open(cmd_file, 'w') as f:
            f.write('#!/usr/bin/env bash\n')
            f.write(out)
        st = os.stat(cmd_file)
        os.chmod(cmd_file, st.st_mode | stat.S_IXUSR)

def run_cmd(cmd, cmd_file=None, out_file=None, extra_env=None, **kwargs):
    """
    Run a command. Write the command to stdout before running it.

    Wait until the command finishes execution.

    If:

    - cmd_file is not None, write the command to the given file
    - out_file is not None, write the stdout and stderr of the command to the given file
    """
    if out_file is not None:
        stdout=subprocess.PIPE
        stderr=subprocess.STDOUT
    else:
        stdout=None
        stderr=None
    if extra_env is None:
        extra_env = {}
    env = os.environ.copy()
    env.update(extra_env)
    print_cmd(cmd, cmd_file, extra_env=extra_env)
    # Otherwise Ctrl + C gives:
    # - ugly Python stack trace for gem5 (QEMU takes over terminal and is fine).
    # - kills Python, and that then kills GDB: https://stackoverflow.com/questions/19807134/does-python-always-raise-an-exception-if-you-do-ctrlc-when-a-subprocess-is-exec
    signal.signal(signal.SIGINT, signal.SIG_IGN)
    # https://stackoverflow.com/questions/15535240/python-popen-write-to-stdout-and-log-file-simultaneously/52090802#52090802
    with subprocess.Popen(cmd, stdout=stdout, stderr=stderr, env=env, **kwargs) as proc:
        if out_file is not None:
            with open(out_file, 'bw') as logfile:
                while True:
                    byte = proc.stdout.read(1)
                    if byte:
                        sys.stdout.buffer.write(byte)
                        sys.stdout.flush()
                        logfile.write(byte)
                    else:
                        break
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    return proc.returncode

def setup(parser, **extra_args):
    """
    Parse the command line arguments, and setup several variables based on them.
    Typically done after getting inputs from the command line arguments.
    """
    global this
    args = parser.parse_args()
    if args.arch in this.arch_map:
        args.arch = this.arch_map[args.arch]
    if args.arch == 'arm':
        this.armv = 7
        this.gem5_arch = 'ARM'
    elif args.arch == 'aarch64':
        this.armv = 8
        this.gem5_arch = 'ARM'
    elif args.arch == 'x86_64':
        this.gem5_arch = 'X86'
    this.buildroot_dir = os.path.join(root_dir, 'buildroot')
    this.arch_dir = args.arch
    if args.suffix is not None:
        this.arch_dir = '{}-{}'.format(arch_dir, args.suffix)
    global out_arch_dir
    this.out_arch_dir = os.path.join(this.out_dir, this.arch_dir)
    this.buildroot_out_dir = os.path.join(this.out_arch_dir, 'buildroot')
    this.build_dir = os.path.join(this.buildroot_out_dir, 'build')
    this.linux_custom_dir = os.path.join(this.build_dir, 'linux-custom')
    this.linux_variant_dir = '{}.{}'.format(this.linux_custom_dir, args.linux_build_id)
    this.vmlinux = os.path.join(this.linux_variant_dir, "vmlinux")
    this.qemu_custom_dir = os.path.join(this.build_dir, 'host-qemu-custom')
    this.qemu_guest_variant_dir = os.path.join(this.qemu_custom_dir, args.qemu_build_id)
    this.qemu_variant_dir = '{}.{}'.format(this.qemu_custom_dir, args.qemu_build_id)
    this.qemu_executable = os.path.join(this.qemu_variant_dir, '{}-softmmu'.format(args.arch), 'qemu-system-{}'.format(args.arch))
    this.qemu_guest_custom_dir = os.path.join(this.build_dir, 'qemu-custom')
    this.host_dir = os.path.join(this.buildroot_out_dir, 'host')
    this.host_bin_dir = os.path.join(this.host_dir, 'usr', 'bin')
    this.images_dir = os.path.join(this.buildroot_out_dir, 'images')
    this.ext2_file = os.path.join(this.images_dir, 'rootfs.ext2')
    this.qcow2_file = os.path.join(this.images_dir, 'rootfs.ext2.qcow2')
    this.staging_dir = os.path.join(this.buildroot_out_dir, 'staging')
    this.target_dir = os.path.join(this.buildroot_out_dir, 'target')
    this.gem5_run_dir = os.path.join(this.out_arch_dir, 'gem5', str(args.run_id))
    this.m5out_dir = os.path.join(this.gem5_run_dir, 'm5out')
    this.stats_file = os.path.join(this.m5out_dir, 'stats.txt')
    this.trace_txt_file = os.path.join(this.m5out_dir, 'trace.txt')
    this.gem5_termout_file = os.path.join(this.gem5_run_dir, 'termout.txt')
    this.qemu_run_dir = os.path.join(this.out_arch_dir, 'qemu', str(args.run_id))
    this.qemu_termout_file = os.path.join(this.qemu_run_dir, 'termout.txt')
    this.qemu_rrfile = os.path.join(this.qemu_run_dir, 'rrfile')
    this.gem5_out_dir = os.path.join(this.common_dir, 'gem5', args.gem5_build_id)
    this.gem5_m5term = os.path.join(this.gem5_out_dir, 'm5term')
    this.gem5_build_dir = os.path.join(this.gem5_out_dir, 'build')
    this.gem5_executable = os.path.join(this.gem5_build_dir, gem5_arch, 'gem5.{}'.format(args.gem5_build_type))
    this.gem5_system_dir = os.path.join(this.gem5_out_dir, 'system')
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
    this.run_cmd_file = os.path.join(this.run_dir, 'cmd.sh')
    if args.arch == 'arm':
        this.linux_image = os.path.join('arch', 'arm', 'boot', 'zImage')
    elif args.arch == 'aarch64':
        this.linux_image = os.path.join('arch', 'arm64', 'boot', 'Image')
    elif args.arch == 'x86_64':
        this.linux_image = os.path.join('arch', 'x86', 'boot', 'bzImage')
    this.linux_image = os.path.join(this.linux_variant_dir, linux_image)

    # Ports.
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
    return args

def mkdir():
    global this
    os.makedirs(this.build_dir, exist_ok=True)
    os.makedirs(this.gem5_out_dir, exist_ok=True)
    os.makedirs(this.gem5_run_dir, exist_ok=True)
    os.makedirs(this.qemu_run_dir, exist_ok=True)
    os.makedirs(this.p9_dir, exist_ok=True)

# Default paths.
root_dir = os.path.dirname(os.path.abspath(__file__))
data_dir = os.path.join(root_dir, 'data')
p9_dir = os.path.join(data_dir, '9p')
gem5_non_default_src_root_dir = os.path.join(data_dir, 'gem5')
gem5_readfile_file = os.path.join(data_dir, 'readfile')
gem5_default_src_dir = os.path.join(root_dir, 'gem5', 'gem5')
out_dir = os.path.join(root_dir, 'out')
bench_boot = os.path.join(out_dir, 'bench-boot.txt')
common_dir = os.path.join(out_dir, 'common')

# Other default variables.
arch_map = {
    'a': 'arm',
    'A': 'aarch64',
    'x': 'x86_64',
}
gem5_cpt_prefix = '^cpt\.'
sha = subprocess.check_output(['git', '-C', root_dir, 'log', '-1', '--format=%H']).decode().rstrip()
config_file = os.path.join(data_dir, 'config')
if os.path.exists(config_file):
    config = imp.load_source('config', config_file)
    configs = {x:getattr(config, x) for x in dir(config) if not x.startswith('__')}
