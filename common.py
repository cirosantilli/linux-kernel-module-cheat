#!/usr/bin/env python3

import argparse
import base64
import collections
import copy
import datetime
import glob
import imp
import inspect
import json
import multiprocessing
import os
import re
import shutil
import signal
import subprocess
import sys
import time
import urllib
import urllib.request

import cli_function
import shell_helpers
from shell_helpers import LF

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
consts['gem5_out_dir'] = os.path.join(consts['out_dir'], 'gem5')
consts['kernel_modules_build_base_dir'] = os.path.join(consts['out_dir'], 'kernel_modules')
consts['buildroot_out_dir'] = os.path.join(consts['out_dir'], 'buildroot')
consts['gem5_m5_build_dir'] = os.path.join(consts['out_dir'], 'util', 'm5')
consts['run_dir_base'] = os.path.join(consts['out_dir'], 'run')
consts['crosstool_ng_out_dir'] = os.path.join(consts['out_dir'], 'crosstool-ng')
consts['bench_boot'] = os.path.join(consts['out_dir'], 'bench-boot.txt')
consts['packages_dir'] = os.path.join(consts['root_dir'], 'buildroot_packages')
consts['kernel_modules_subdir'] = 'kernel_modules'
consts['kernel_modules_src_dir'] = os.path.join(consts['root_dir'], consts['kernel_modules_subdir'])
consts['userland_subdir'] = 'userland'
consts['userland_source_dir'] = os.path.join(consts['root_dir'], consts['userland_subdir'])
consts['userland_build_ext'] = '.out'
consts['include_subdir'] = 'include'
consts['include_src_dir'] = os.path.join(consts['root_dir'], consts['include_subdir'])
consts['submodules_dir'] = os.path.join(consts['root_dir'], 'submodules')
consts['buildroot_src_dir'] = os.path.join(consts['submodules_dir'], 'buildroot')
consts['crosstool_ng_src_dir'] = os.path.join(consts['submodules_dir'], 'crosstool-ng')
consts['crosstool_ng_supported_archs'] = set(['arm', 'aarch64'])
consts['linux_source_dir'] = os.path.join(consts['submodules_dir'], 'linux')
consts['linux_config_dir'] = os.path.join(consts['root_dir'], 'linux_config')
consts['gem5_default_src_dir'] = os.path.join(consts['submodules_dir'], 'gem5')
consts['rootfs_overlay_dir'] = os.path.join(consts['root_dir'], 'rootfs_overlay')
consts['extract_vmlinux'] = os.path.join(consts['linux_source_dir'], 'scripts', 'extract-vmlinux')
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
def git_sha(repo_path):
    return subprocess.check_output(['git', '-C', repo_path, 'log', '-1', '--format=%H']).decode().rstrip()
consts['sha'] = common.git_sha(consts['root_dir'])
consts['release_dir'] = os.path.join(consts['out_dir'], 'release')
consts['release_zip_file'] = os.path.join(consts['release_dir'], 'lkmc-{}.zip'.format(consts['sha']))
consts['github_repo_id'] = 'cirosantilli/linux-kernel-module-cheat'
consts['asm_ext'] = '.S'
consts['c_ext'] = '.c'
consts['header_ext'] = '.h'
consts['kernel_module_ext'] = '.ko'
consts['obj_ext'] = '.o'
consts['config_file'] = os.path.join(consts['data_dir'], 'config.py')
consts['magic_fail_string'] = b'lkmc_test_fail'
consts['baremetal_lib_basename'] = 'lib'
consts['emulators'] = ['qemu', 'gem5']

class LkmcCliFunction(cli_function.CliFunction):
    '''
    Common functionality shared across our CLI functions:

    * command timing
    * some common flags, e.g.: --arch, --dry-run, --verbose
    '''
    def __init__(self, *args, defaults=None, **kwargs):
        '''
        :ptype defaults: Dict[str,Any]
        :param defaults: override the default value of an argument
        '''
        kwargs['config_file'] = consts['config_file']
        kwargs['extra_config_params'] = os.path.basename(inspect.getfile(self.__class__))
        if defaults is None:
            defaults = {}
        self._defaults = defaults
        super().__init__(*args, **kwargs)

        # Args for all scripts.
        self.add_argument(
            '-a', '--arch', choices=consts['arch_choices'], default=consts['default_arch'],
            help='CPU architecture.'
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
            '--print-time', default=True,
            help='Print how long it took to run the command at the end.'
        )
        self.add_argument(
            '-v', '--verbose', default=False,
            help='Show full compilation commands when they are not shown by default.'
        )

        # Gem5 args.
        self.add_argument(
            '--gem5-build-dir',
            help='''\
Use the given directory as the gem5 build directory. Ignore --gem5-build-id and --gem5-build-type.
'''
        )
        self.add_argument(
            '-M', '--gem5-build-id', default='default',
            help='''\
gem5 build ID. Allows you to keep multiple separate gem5 builds.
'''
        )
        self.add_argument(
            '--gem5-build-type', default='opt',
            help='gem5 build type, most often used for "debug" builds.'
        )
        self.add_argument(
            '--gem5-source-dir',
            help='''\
Use the given directory as the gem5 source tree. Ignore `--gem5-worktree`.
'''
        )
        self.add_argument(
            '-N', '--gem5-worktree',
            help='''\
Create and use a git worktree of the gem5 submodule.
See: https://github.com/cirosantilli/linux-kernel-module-cheat#gem5-worktree
'''
        )

        # Linux kernel.
        self.add_argument(
            '--linux-build-dir',
            help='''\
Use the given directory as the Linux build directory. Ignore --linux-build-id.
'''
        )
        self.add_argument(
            '-L', '--linux-build-id', default=consts['default_build_id'],
            help='''\
Linux build ID. Allows you to keep multiple separate Linux builds.
'''
        )
        self.add_argument(
            '--linux-source-dir',
            help='''\
Use the given directory as the Linux source tree.
'''
        )
        self.add_argument(
            '--initramfs', default=False,
        )
        self.add_argument(
            '--initrd', default=False,
        )

        # Baremetal.
        self.add_argument(
            '-b', '--baremetal',
            help='''\
Use the given baremetal executable instead of the Linux kernel.

If the path is absolute, it is used as is.

If the path is relative, we assume that it points to a source code
inside baremetal/ and then try to use corresponding executable.
'''
        )

        # Buildroot.
        self.add_argument(
            '--buildroot-build-id',
            default=consts['default_build_id'],
            help='Buildroot build ID. Allows you to keep multiple separate gem5 builds.'
        )
        self.add_argument(
            '--buildroot-linux', default=False,
            help='Boot with the Buildroot Linux kernel instead of our custom built one. Mostly for sanity checks.'
        )

        # crosstool-ng
        self.add_argument(
            '--crosstool-ng-build-id', default=consts['default_build_id'],
            help='Crosstool-NG build ID. Allows you to keep multiple separate crosstool-NG builds.'
        )
        self.add_argument(
            '--docker', default=False,
            help='''\
Use the docker download Ubuntu root filesystem instead of the default Buildroot one.
'''
        )

        self.add_argument(
            '--machine',
            help='''Machine type.
QEMU default: virt
gem5 default: VExpress_GEM5_V1
See the documentation for other values known to work.
'''
        )

        # QEMU.
        self.add_argument(
            '-Q', '--qemu-build-id', default=consts['default_build_id'],
            help='QEMU build ID. Allows you to keep multiple separate QEMU builds.'
        )

        # Userland.
        self.add_argument(
            '--userland-build-id', default=None
        )

        # Run.
        self.add_argument(
            '-n', '--run-id', default='0',
            help='''\
ID for run outputs such as gem5's m5out. Allows you to do multiple runs,
and then inspect separate outputs later in different output directories.
'''
        )
        self.add_argument(
            '-P', '--prebuilt', default=False,
            help='''\
Use prebuilt packaged host utilities as much as possible instead
of the ones we built ourselves. Saves build time, but decreases
the likelihood of incompatibilities.
'''
        )
        self.add_argument(
            '--port-offset', type=int,
            help='''\
Increase the ports to be used such as for GDB by an offset to run multiple
instances in parallel. Default: the run ID (-n) if that is an integer, otherwise 0.
'''
        )

        # Misc.
        self.add_argument(
            '--emulator', choices=consts['emulators'],
            help='''\
Set the emulator to use. Ignore --gem5.
'''
        )
        self.add_argument(
            '-g', '--gem5', default=False,
            help='''\
Use gem5 instead of QEMU. Shortcut for `--emulator gem5`.
'''
        )

    def __call__(self, **kwargs):
        '''
        For Python code calls, print the CLI equivalent of the call.
        '''
        print_cmd = ['./' + self.extra_config_params, LF]
        for line in self.get_cli(**kwargs):
            print_cmd.extend(line)
            print_cmd.append(LF)
        shell_helpers.ShellHelpers.print_cmd(print_cmd)
        return super().__call__(**kwargs)

    def _init_env(self, env):
        '''
        Update the kwargs from the command line with values derived from them.
        '''
        def join(*paths):
            return os.path.join(*paths)
        if env['emulator'] is None:
            if env['gem5']:
                env['emulator'] = 'gem5'
            else:
                env['emulator'] = 'qemu'
        if env['arch'] in env['arch_short_to_long_dict']:
            env['arch'] = env['arch_short_to_long_dict'][env['arch']]
        if env['userland_build_id'] is None:
            env['userland_build_id'] = env['default_build_id']
            env['userland_build_id_given'] = False
        else:
            env['userland_build_id_given'] = True
        if env['gem5_worktree'] is not None and env['gem5_build_id'] is None:
            env['gem5_build_id'] = env['gem5_worktree']
        env['is_arm'] = False
        if env['arch'] == 'arm':
            env['armv'] = 7
            env['gem5_arch'] = 'ARM'
            env['mcpu'] = 'cortex-a15'
            env['buildroot_toolchain_prefix'] = 'arm-buildroot-linux-uclibcgnueabihf'
            env['crosstool_ng_toolchain_prefix'] = 'arm-unknown-eabi'
            env['ubuntu_toolchain_prefix'] = 'arm-linux-gnueabihf'
            if env['emulator'] == 'gem5':
                if env['machine'] is None:
                    env['machine'] = 'VExpress_GEM5_V1'
            else:
                if env['machine'] is None:
                    env['machine'] = 'virt'
            env['is_arm'] = True
        elif env['arch'] == 'aarch64':
            env['armv'] = 8
            env['gem5_arch'] = 'ARM'
            env['mcpu'] = 'cortex-a57'
            env['buildroot_toolchain_prefix'] = 'aarch64-buildroot-linux-uclibc'
            env['crosstool_ng_toolchain_prefix'] = 'aarch64-unknown-elf'
            env['ubuntu_toolchain_prefix'] = 'aarch64-linux-gnu'
            if env['emulator'] == 'gem5':
                if env['machine'] is None:
                    env['machine'] = 'VExpress_GEM5_V1'
            else:
                if env['machine'] is None:
                    env['machine'] = 'virt'
            env['is_arm'] = True
        elif env['arch'] == 'x86_64':
            env['crosstool_ng_toolchain_prefix'] = 'x86_64-unknown-elf'
            env['gem5_arch'] = 'X86'
            env['buildroot_toolchain_prefix'] = 'x86_64-buildroot-linux-uclibc'
            env['ubuntu_toolchain_prefix'] = 'x86_64-linux-gnu'
            if env['emulator'] == 'gem5':
                if env['machine'] is None:
                    env['machine'] = 'TODO'
            else:
                if env['machine'] is None:
                    env['machine'] = 'pc'

        # Buildroot
        env['buildroot_build_dir'] = join(env['buildroot_out_dir'], 'build', env['buildroot_build_id'], env['arch'])
        env['buildroot_download_dir'] = join(env['buildroot_out_dir'], 'download')
        env['buildroot_config_file'] = join(env['buildroot_build_dir'], '.config')
        env['buildroot_build_build_dir'] = join(env['buildroot_build_dir'], 'build')
        env['buildroot_linux_build_dir'] = join(env['buildroot_build_build_dir'], 'linux-custom')
        env['buildroot_vmlinux'] = join(env['buildroot_linux_build_dir'], "vmlinux")
        env['host_dir'] = join(env['buildroot_build_dir'], 'host')
        env['host_bin_dir'] = join(env['host_dir'], 'usr', 'bin')
        env['buildroot_pkg_config'] = join(env['host_bin_dir'], 'pkg-config')
        env['buildroot_images_dir'] = join(env['buildroot_build_dir'], 'images')
        env['buildroot_rootfs_raw_file'] = join(env['buildroot_images_dir'], 'rootfs.ext2')
        env['buildroot_qcow2_file'] = env['buildroot_rootfs_raw_file'] + '.qcow2'
        env['staging_dir'] = join(env['out_dir'], 'staging', env['arch'])
        env['buildroot_staging_dir'] = join(env['buildroot_build_dir'], 'staging')
        env['target_dir'] = join(env['buildroot_build_dir'], 'target')
        if env['linux_source_dir'] is None:
            env['linux_source_dir'] = os.path.join(consts['submodules_dir'], 'linux')
        common.extract_vmlinux = os.path.join(env['linux_source_dir'], 'scripts', 'extract-vmlinux')
        env['linux_buildroot_build_dir'] = join(env['buildroot_build_build_dir'], 'linux-custom')

        # QEMU
        env['qemu_build_dir'] = join(env['out_dir'], 'qemu', env['qemu_build_id'])
        env['qemu_executable_basename'] = 'qemu-system-{}'.format(env['arch'])
        env['qemu_executable'] = join(env['qemu_build_dir'], '{}-softmmu'.format(env['arch']), env['qemu_executable_basename'])
        env['qemu_img_basename'] = 'qemu-img'
        env['qemu_img_executable'] = join(env['qemu_build_dir'], env['qemu_img_basename'])

        # gem5
        if env['gem5_build_dir'] is None:
            env['gem5_build_dir'] = join(env['gem5_out_dir'], env['gem5_build_id'], env['gem5_build_type'])
        env['gem5_fake_iso'] = join(env['gem5_out_dir'], 'fake.iso')
        env['gem5_m5term'] = join(env['gem5_build_dir'], 'm5term')
        env['gem5_build_build_dir'] = join(env['gem5_build_dir'], 'build')
        env['gem5_executable'] = join(env['gem5_build_build_dir'], env['gem5_arch'], 'gem5.{}'.format(env['gem5_build_type']))
        env['gem5_system_dir'] = join(env['gem5_build_dir'], 'system')

        # gem5 source
        if env['gem5_source_dir'] is not None:
            assert os.path.exists(env['gem5_source_dir'])
        else:
            if env['gem5_worktree'] is not None:
                env['gem5_source_dir'] = join(env['gem5_non_default_src_root_dir'], env['gem5_worktree'])
            else:
                env['gem5_source_dir'] = env['gem5_default_src_dir']
        env['gem5_m5_source_dir'] = join(env['gem5_source_dir'], 'util', 'm5')
        env['gem5_config_dir'] = join(env['gem5_source_dir'], 'configs')
        env['gem5_se_file'] = join(env['gem5_config_dir'], 'example', 'se.py')
        env['gem5_fs_file'] = join(env['gem5_config_dir'], 'example', 'fs.py')

        # crosstool-ng
        env['crosstool_ng_buildid_dir'] = join(env['crosstool_ng_out_dir'], 'build', env['crosstool_ng_build_id'])
        env['crosstool_ng_install_dir'] = join(env['crosstool_ng_buildid_dir'], 'install', env['arch'])
        env['crosstool_ng_bin_dir'] = join(env['crosstool_ng_install_dir'], 'bin')
        env['crosstool_ng_util_dir'] = join(env['crosstool_ng_buildid_dir'], 'util')
        env['crosstool_ng_config'] = join(env['crosstool_ng_util_dir'], '.config')
        env['crosstool_ng_defconfig'] = join(env['crosstool_ng_util_dir'], 'defconfig')
        env['crosstool_ng_executable'] = join(env['crosstool_ng_util_dir'], 'ct-ng')
        env['crosstool_ng_build_dir'] = join(env['crosstool_ng_buildid_dir'], 'build')
        env['crosstool_ng_download_dir'] = join(env['crosstool_ng_out_dir'], 'download')

        # run
        env['gem5_run_dir'] = join(env['run_dir_base'], 'gem5', env['arch'], str(env['run_id']))
        env['m5out_dir'] = join(env['gem5_run_dir'], 'm5out')
        env['stats_file'] = join(env['m5out_dir'], 'stats.txt')
        env['gem5_trace_txt_file'] = join(env['m5out_dir'], 'trace.txt')
        env['gem5_guest_terminal_file'] = join(env['m5out_dir'], 'system.terminal')
        env['gem5_readfile'] = join(env['gem5_run_dir'], 'readfile')
        env['gem5_termout_file'] = join(env['gem5_run_dir'], 'termout.txt')
        env['qemu_run_dir'] = join(env['run_dir_base'], 'qemu', env['arch'], str(env['run_id']))
        env['qemu_termout_file'] = join(env['qemu_run_dir'], 'termout.txt')
        env['qemu_trace_basename'] = 'trace.bin'
        env['qemu_trace_file'] = join(env['qemu_run_dir'], 'trace.bin')
        env['qemu_trace_txt_file'] = join(env['qemu_run_dir'], 'trace.txt')
        env['qemu_rrfile'] = join(env['qemu_run_dir'], 'rrfile')
        env['gem5_out_dir'] = join(env['out_dir'], 'gem5')

        # Ports
        if env['port_offset'] is None:
            try:
                env['port_offset'] = int(env['run_id'])
            except ValueError:
                env['port_offset'] = 0
        if env['emulator'] == 'gem5':
            env['gem5_telnet_port'] = 3456 + env['port_offset']
            env['gdb_port'] = 7000 + env['port_offset']
        else:
            env['qemu_base_port'] = 45454 + 10 * env['port_offset']
            env['qemu_monitor_port'] = env['qemu_base_port'] + 0
            env['qemu_hostfwd_generic_port'] = env['qemu_base_port'] + 1
            env['qemu_hostfwd_ssh_port'] = env['qemu_base_port'] + 2
            env['qemu_gdb_port'] = env['qemu_base_port'] + 3
            env['extra_serial_port'] = env['qemu_base_port'] + 4
            env['gdb_port'] = env['qemu_gdb_port']
            env['qemu_background_serial_file'] = join(env['qemu_run_dir'], 'background.log')

        # gem5 QEMU polymorphism.
        if env['emulator'] == 'gem5':
            env['executable'] = env['gem5_executable']
            env['run_dir'] = env['gem5_run_dir']
            env['termout_file'] = env['gem5_termout_file']
            env['guest_terminal_file'] = env['gem5_guest_terminal_file']
            env['trace_txt_file'] = env['gem5_trace_txt_file']
        else:
            env['executable'] = env['qemu_executable']
            env['run_dir'] = env['qemu_run_dir']
            env['termout_file'] = env['qemu_termout_file']
            env['guest_terminal_file'] = env['qemu_termout_file']
            env['trace_txt_file'] = env['qemu_trace_txt_file']
        env['run_cmd_file'] = join(env['run_dir'], 'run.sh')

        # Linux kernl.
        if env['linux_build_dir'] is None:
            env['linux_build_dir'] = join(env['out_dir'], 'linux', env['linux_build_id'], env['arch'])
        env['lkmc_vmlinux'] = join(env['linux_build_dir'], "vmlinux")
        if env['arch'] == 'arm':
            env['linux_arch'] = 'arm'
            env['linux_image_prefix'] = join('arch', env['linux_arch'], 'boot', 'zImage')
        elif env['arch'] == 'aarch64':
            env['linux_arch'] = 'arm64'
            env['linux_image_prefix'] = join('arch', env['linux_arch'], 'boot', 'Image')
        elif env['arch'] == 'x86_64':
            env['linux_arch'] = 'x86'
            env['linux_image_prefix'] = join('arch', env['linux_arch'], 'boot', 'bzImage')
        env['lkmc_linux_image'] = join(env['linux_build_dir'], env['linux_image_prefix'])
        env['buildroot_linux_image'] = join(env['buildroot_linux_build_dir'], env['linux_image_prefix'])
        if env['buildroot_linux']:
            env['vmlinux'] = env['buildroot_vmlinux']
            env['linux_image'] = env['buildroot_linux_image']
        else:
            env['vmlinux'] = env['lkmc_vmlinux']
            env['linux_image'] = env['lkmc_linux_image']

        # Kernel modules.
        env['kernel_modules_build_dir'] = join(env['kernel_modules_build_base_dir'], env['arch'])
        env['kernel_modules_build_subdir'] = join(env['kernel_modules_build_dir'], env['kernel_modules_subdir'])
        env['kernel_modules_build_host_dir'] = join(env['kernel_modules_build_base_dir'], 'host')
        env['kernel_modules_build_host_subdir'] = join(env['kernel_modules_build_host_dir'], env['kernel_modules_subdir'])
        env['userland_build_dir'] = join(env['out_dir'], 'userland', env['userland_build_id'], env['arch'])
        env['out_rootfs_overlay_dir'] = join(env['out_dir'], 'rootfs_overlay', env['arch'])
        env['out_rootfs_overlay_bin_dir'] = join(env['out_rootfs_overlay_dir'], 'bin')

        # Baremetal.
        env['baremetal_src_dir'] = join(env['root_dir'], 'baremetal')
        env['baremetal_src_arch_subpath'] = join('arch', env['arch'])
        env['baremetal_src_arch_dir'] = join(env['baremetal_src_dir'], env['baremetal_src_arch_subpath'])
        env['baremetal_src_lib_dir'] = join(env['baremetal_src_dir'], env['baremetal_lib_basename'])
        if env['emulator'] == 'gem5':
            env['simulator_name'] = 'gem5'
        else:
            env['simulator_name'] = 'qemu'
        env['baremetal_build_dir'] = join(env['out_dir'], 'baremetal', env['arch'], env['simulator_name'], env['machine'])
        env['baremetal_build_lib_dir'] = join(env['baremetal_build_dir'], env['baremetal_lib_basename'])
        env['baremetal_build_ext'] = '.elf'

        # Docker
        env['docker_build_dir'] = join(env['out_dir'], 'docker', env['arch'])
        env['docker_tar_dir'] = join(env['docker_build_dir'], 'export')
        env['docker_tar_file'] = join(env['docker_build_dir'], 'export.tar')
        env['docker_rootfs_raw_file'] = join(env['docker_build_dir'], 'export.ext2')
        env['docker_qcow2_file'] = join(env['docker_rootfs_raw_file'] + '.qcow2')
        if env['docker']:
            env['rootfs_raw_file'] = env['docker_rootfs_raw_file']
            env['qcow2_file'] = env['docker_qcow2_file']
        else:
            env['rootfs_raw_file'] = env['buildroot_rootfs_raw_file']
            env['qcow2_file'] = env['buildroot_qcow2_file']

        # Image
        if env['baremetal'] is None:
            if env['emulator'] == 'gem5':
                env['image'] = env['vmlinux']
                env['disk_image'] = env['rootfs_raw_file']
            else:
                env['image'] = env['linux_image']
                env['disk_image'] = env['qcow2_file']
        else:
            env['disk_image'] = env['gem5_fake_iso']
            if env['baremetal'] == 'all':
                path = env['baremetal']
            else:
                path = self.resolve_executable(
                    env['baremetal'],
                    env['baremetal_src_dir'],
                    env['baremetal_build_dir'],
                    env['baremetal_build_ext'],
                )
                source_path_noext = os.path.splitext(join(
                    env['baremetal_src_dir'],
                    os.path.relpath(path, env['baremetal_build_dir'])
                ))[0]
                for ext in [env['c_ext'], env['asm_ext']]:
                    source_path = source_path_noext + ext
                    if os.path.exists(source_path):
                        env['source_path'] = source_path
                        break
            env['image'] = path

        self.env = env


    def add_argument(self, *args, **kwargs):
        shortname, longname, key, is_option = self.get_key(*args, **kwargs)
        if key in self._defaults:
            kwargs['default'] = self._defaults[key]
        super().add_argument(*args, **kwargs)

    def assert_crosstool_ng_supports_arch(self, arch):
        if arch not in self.env['crosstool_ng_supported_archs']:
            raise Exception('arch not yet supported: ' + arch)

    @staticmethod
    def base64_encode(string):
        return base64.b64encode(string.encode()).decode()

    def gem5_list_checkpoint_dirs(self):
        '''
        List checkpoint directory, oldest first.
        '''
        prefix_re = re.compile(self.env['gem5_cpt_prefix'])
        files = list(filter(lambda x: os.path.isdir(os.path.join(self.env['m5out_dir'], x)) and prefix_re.search(x), os.listdir(self.env['m5out_dir'])))
        files.sort(key=lambda x: os.path.getmtime(os.path.join(self.env['m5out_dir'], x)))
        return files

    def get_elf_entry(self, elf_file_path):
        readelf_header = subprocess.check_output([
            self.get_toolchain_tool('readelf'),
            '-h',
            elf_file_path
        ])
        for line in readelf_header.decode().split('\n'):
            split = line.split()
            if line.startswith('  Entry point address:'):
                addr = line.split()[-1]
                break
        return int(addr, 0)

    def get_stats(self, stat_re=None, stats_file=None):
        if stat_re is None:
            stat_re = '^system.cpu[0-9]*.numCycles$'
        if stats_file is None:
            stats_file = self.env['stats_file']
        stat_re = re.compile(stat_re)
        ret = []
        with open(stats_file, 'r') as statfile:
            for line in statfile:
                if line[0] != '-':
                    cols = line.split()
                    if len(cols) > 1 and stat_re.search(cols[0]):
                        ret.append(cols[1])
        return ret

    def get_toolchain_prefix(self, tool, allowed_toolchains=None):
        buildroot_full_prefix = os.path.join(self.env['host_bin_dir'], self.env['buildroot_toolchain_prefix'])
        buildroot_exists = os.path.exists('{}-{}'.format(buildroot_full_prefix, tool))
        crosstool_ng_full_prefix = os.path.join(self.env['crosstool_ng_bin_dir'], self.env['crosstool_ng_toolchain_prefix'])
        crosstool_ng_exists = os.path.exists('{}-{}'.format(crosstool_ng_full_prefix, tool))
        host_tool = '{}-{}'.format(self.env['ubuntu_toolchain_prefix'], tool)
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
            if self.env['baremetal'] is None:
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

    def get_toolchain_tool(self, tool, allowed_toolchains=None):
        return '{}-{}'.format(self.get_toolchain_prefix(tool, allowed_toolchains), tool)

    @staticmethod
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

    @staticmethod
    def import_path(path):
        '''
        https://stackoverflow.com/questions/2601047/import-a-python-module-without-the-py-extension
        https://stackoverflow.com/questions/31773310/what-does-the-first-argument-of-the-imp-load-source-method-do
        '''
        return imp.load_source(os.path.split(path)[1].replace('-', '_'), path)

    @staticmethod
    def log_error(msg):
        print('error: {}'.format(msg), file=sys.stderr)

    def main(self, **kwargs):
        '''
        Time the main of the derived class.
        '''
        myargs = kwargs.copy()
        if not myargs['dry_run']:
            start_time = time.time()
        myargs.update(consts)
        self._init_env(myargs)
        self.sh = shell_helpers.ShellHelpers(dry_run=self.env['dry_run'])
        ret = self.timed_main()
        if not myargs['dry_run']:
            end_time = time.time()
            self._print_time(end_time - start_time)
        return ret

    def make_build_dirs(self):
        os.makedirs(self.env['buildroot_build_build_dir'], exist_ok=True)
        os.makedirs(self.env['gem5_build_dir'], exist_ok=True)
        os.makedirs(self.env['out_rootfs_overlay_dir'], exist_ok=True)

    def make_run_dirs(self):
        '''
        Make directories required for the run.
        The user could nuke those anytime between runs to try and clean things up.
        '''
        os.makedirs(self.env['gem5_run_dir'], exist_ok=True)
        os.makedirs(self.env['p9_dir'], exist_ok=True)
        os.makedirs(self.env['qemu_run_dir'], exist_ok=True)

    @staticmethod
    def need_rebuild(srcs, dst):
        if not os.path.exists(dst):
            return True
        for src in srcs:
            if os.path.getmtime(src) > os.path.getmtime(dst):
                return True
        return False

    def _print_time(self, ellapsed_seconds):
        if self.env['print_time']:
            hours, rem = divmod(ellapsed_seconds, 3600)
            minutes, seconds = divmod(rem, 60)
            print("time {:02}:{:02}:{:02}".format(int(hours), int(minutes), int(seconds)))

    def raw_to_qcow2(self, prebuilt=False, reverse=False):
        if prebuilt or not os.path.exists(self.env['qemu_img_executable']):
            disable_trace = []
            qemu_img_executable = self.env['qemu_img_basename']
        else:
            # Prevent qemu-img from generating trace files like QEMU. Disgusting.
            disable_trace = ['-T', 'pr_manager_run,file=/dev/null', LF]
            qemu_img_executable = self.env['qemu_img_executable']
        infmt = 'raw'
        outfmt = 'qcow2'
        infile = self.env['rootfs_raw_file']
        outfile = self.env['qcow2_file']
        if reverse:
            tmp = infmt
            infmt = outfmt
            outfmt = tmp
            tmp = infile
            infile = outfile
            outfile = tmp
        self.sh.run_cmd(
            [
                qemu_img_executable, LF,
            ] +
            disable_trace +
            [
                'convert', LF,
                '-f', infmt, LF,
                '-O', outfmt, LF,
                infile, LF,
                outfile, LF,
            ]
        )

    @staticmethod
    def resolve_args(defaults, args, extra_args):
        if extra_args is None:
            extra_args = {}
        argcopy = copy.copy(args)
        argcopy.__dict__ = dict(list(defaults.items()) + list(argcopy.__dict__.items()) + list(extra_args.items()))
        return argcopy

    @staticmethod
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

    def resolve_userland(self, path):
        return self.resolve_executable(
            path,
            self.env['userland_source_dir'],
            self.env['userland_build_dir'],
            self.env['userland_build_ext'],
        )

    def timed_main(self):
        '''
        Main action of the derived class.
        '''
        raise NotImplementedError()

class BuildCliFunction(LkmcCliFunction):
    '''
    A CLI function with common facilities to build stuff, e.g.:

    * `--clean` to clean the build directory
    * `--nproc` to set he number of build threads
    '''
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
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
            help='Number of processors to use for the build.',
        )

    def clean(self):
        build_dir = self.get_build_dir()
        if build_dir is not None:
            self.sh.rmrf(build_dir)

    def build(self):
        '''
        Do the actual main build work.
        '''
        raise NotImplementedError()

    def get_build_dir(self):
        return None

    def timed_main(self):
        '''
        Parse CLI, and to the build based on it.

        The actual build work is done by do_build in implementing classes.
        '''
        if self.env['clean']:
            return self.clean()
        else:
            return self.build()
