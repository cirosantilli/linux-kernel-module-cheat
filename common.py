#!/usr/bin/env python3

import argparse
import imp
import subprocess
import os
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

def get_argparse(**kwargs):
    """
    Return an argument parser with common arguments set.
    """
    global this
    arch_choices = []
    for key in this.arch_map:
        arch_choices.append(key)
        arch_choices.append(this.arch_map[key])
    default_build_id='default'
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter,
        **kwargs
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
gem5 worktree to use for build and Python scripts at runtime.
If not given, just use the submodule source.
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
    # A bit ugly as it actually changes the defaults shown on --help, but we can't do any better
    # because it is impossible to check if arguments were given or not...
    # - https://stackoverflow.com/questions/30487767/check-if-argparse-optional-argument-is-set-or-not
    # - https://stackoverflow.com/questions/3609852/which-is-the-best-way-to-allow-configuration-options-be-overridden-at-the-comman
    parser.set_defaults(**this.configs)
    return parser

def setup(parser):
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
    this.images_dir = os.path.join(this.buildroot_out_dir, 'images')
    this.ext2_file = os.path.join(this.images_dir, 'rootfs.ext2')
    this.qcow2_file = os.path.join(this.images_dir, 'rootfs.ext2.qcow2')
    this.staging_dir = os.path.join(this.buildroot_out_dir, 'staging')
    this.target_dir = os.path.join(this.buildroot_out_dir, 'target')
    this.gem5_run_dir = os.path.join(this.out_arch_dir, 'gem5', str(args.run_id))
    this.m5out_dir = os.path.join(this.gem5_run_dir, 'm5out')
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
    os.mkdirs(this.gem5_out_dir, exit_ok=True)
    os.mkdirs(this.gem5_run_dir, exit_ok=True)
    os.mkdirs(this.qemu_run_dir, exit_ok=True)
    os.mkdirs(this.p9_dir, exit_ok=True)

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
gem5_cpt_pref = '^cpt\.'
sha = subprocess.check_output(['git', '-C', root_dir, 'log', '-1', '--format=%H']).decode().rstrip()

# Config file. TODO move to decent python setup.
config_file = os.path.join(data_dir, 'config')
config = imp.load_source('config', config_file)
configs = {x:getattr(config, x) for x in dir(config) if not x.startswith('__')}
# if os.path.exists(config_file):
    # exec(open(config_file).read())
    # with open(config_file) as f:
        # exec(f.read())
