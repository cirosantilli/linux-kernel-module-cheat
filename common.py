#!/usr/bin/env python3

import argparse
import bisect
import collections
import copy
import datetime
import enum
import functools
import glob
import inspect
import itertools
import json
import math
import os
import platform
import pathlib
import queue
import re
import shutil
import signal
import subprocess
import sys
import threading
from typing import Union
import time
import urllib
import urllib.request

from shell_helpers import LF
import cli_function
import path_properties
import shell_helpers
import thread_pool

common = sys.modules[__name__]

# Fixed parameters that don't depend on CLI arguments.
consts = {}
consts['repo_short_id'] = 'lkmc'
consts['linux_kernel_version'] = '5.4.3'
# https://stackoverflow.com/questions/20010199/how-to-determine-if-a-process-runs-inside-lxc-docker
consts['in_docker'] = os.path.exists('/.dockerenv')
consts['root_dir'] = os.path.dirname(os.path.abspath(__file__))
consts['data_dir'] = os.path.join(consts['root_dir'], 'data')
consts['p9_dir'] = os.path.join(consts['data_dir'], '9p')
consts['gem5_non_default_source_root_dir'] = os.path.join(consts['data_dir'], 'gem5')
if consts['in_docker']:
    consts['out_dir'] = os.path.join(consts['root_dir'], 'out.docker')
else:
    consts['out_dir'] = os.path.join(consts['root_dir'], 'out')
consts['readme'] = os.path.join(consts['root_dir'], 'README.adoc')
consts['readme_out'] = os.path.join(consts['out_dir'], 'README.html')
consts['build_doc_log'] = os.path.join(consts['out_dir'], 'build-doc.log')
consts['gem5_out_dir'] = os.path.join(consts['out_dir'], 'gem5')
consts['kernel_modules_build_base_dir'] = os.path.join(consts['out_dir'], 'kernel_modules')
consts['buildroot_out_dir'] = os.path.join(consts['out_dir'], 'buildroot')
consts['gem5_m5_build_dir'] = os.path.join(consts['out_dir'], 'util', 'm5')
consts['run_dir_base'] = os.path.join(consts['out_dir'], 'run')
consts['crosstool_ng_out_dir'] = os.path.join(consts['out_dir'], 'crosstool-ng')
consts['test_boot_benchmark_file'] = os.path.join(consts['out_dir'], 'test-boot.txt')
consts['packages_dir'] = os.path.join(consts['root_dir'], 'buildroot_packages')
consts['kernel_modules_subdir'] = 'kernel_modules'
consts['kernel_modules_source_dir'] = os.path.join(consts['root_dir'], consts['kernel_modules_subdir'])
consts['userland_subdir'] = 'userland'
consts['userland_source_dir'] = os.path.join(consts['root_dir'], consts['userland_subdir'])
consts['userland_source_arch_dir'] = os.path.join(consts['userland_source_dir'], 'arch')
consts['userland_executable_ext'] = '.out'
consts['baremetal_executable_ext'] = '.elf'
consts['include_subdir'] = consts['repo_short_id']
consts['include_source_dir'] = os.path.join(consts['root_dir'], consts['include_subdir'])
consts['submodules_dir'] = os.path.join(consts['root_dir'], 'submodules')
consts['buildroot_source_dir'] = os.path.join(consts['submodules_dir'], 'buildroot')
consts['crosstool_ng_source_dir'] = os.path.join(consts['submodules_dir'], 'crosstool-ng')
consts['crosstool_ng_supported_archs'] = set(['arm', 'aarch64'])
consts['linux_source_dir'] = os.path.join(consts['submodules_dir'], 'linux')
consts['linux_config_dir'] = os.path.join(consts['root_dir'], 'linux_config')
consts['gem5_default_source_dir'] = os.path.join(consts['submodules_dir'], 'gem5')
consts['rootfs_overlay_dir'] = os.path.join(consts['root_dir'], 'rootfs_overlay')
consts['extract_vmlinux'] = os.path.join(consts['linux_source_dir'], 'scripts', 'extract-vmlinux')
consts['qemu_source_dir'] = os.path.join(consts['submodules_dir'], 'qemu')
consts['parsec_benchmark_source_dir'] = os.path.join(consts['submodules_dir'], 'parsec-benchmark')
consts['ccache_dir'] = os.path.join('/usr', 'lib', 'ccache')
consts['default_build_id'] = 'default'
consts['arch_short_to_long_dict'] = collections.OrderedDict([
    ('x', 'x86_64'),
    ('a', 'arm'),
    ('A', 'aarch64'),
])
# All long arch names.
consts['all_long_archs'] = [consts['arch_short_to_long_dict'][k] for k in consts['arch_short_to_long_dict']]
# All long and short arch names.
consts['arch_choices'] = set()
for key in consts['arch_short_to_long_dict']:
    consts['arch_choices'].add(key)
    consts['arch_choices'].add(consts['arch_short_to_long_dict'][key])
consts['default_arch'] = 'x86_64'
consts['gem5_cpt_prefix'] = '^cpt\.'
def git_sha(repo_path):
    return subprocess.check_output(['git', '-C', repo_path, 'log', '-1', '--format=%H']).decode().rstrip()
consts['sha'] = common.git_sha(consts['root_dir'])
consts['release_dir'] = os.path.join(consts['out_dir'], 'release')
consts['release_zip_file'] = os.path.join(consts['release_dir'], 'lkmc-{}.zip'.format(consts['sha']))
consts['github_repo_id'] = 'cirosantilli/linux-kernel-module-cheat'
consts['github_repo_url'] = 'https://github.com/' + consts['github_repo_id']
consts['homepage_url'] = 'https://cirosantilli.com/linux-kernel-module-cheat'
consts['asm_ext'] = '.S'
consts['c_ext'] = '.c'
consts['cxx_ext'] = '.cpp'
consts['header_ext'] = '.h'
consts['kernel_module_ext'] = '.ko'
consts['obj_ext'] = '.o'
# https://cirosantilli.com/linux-kernel-module-cheat#baremetal-cpp
consts['baremetal_build_in_exts'] = [
    consts['asm_ext'],
    consts['c_ext'],
]
consts['build_in_exts'] = consts['baremetal_build_in_exts'] + [
    consts['cxx_ext']
]
consts['userland_out_exts'] = [
    consts['userland_executable_ext'],
    consts['obj_ext'],
]
consts['default_config_file'] = os.path.join(consts['data_dir'], 'config.py')
consts['serial_magic_exit_status_regexp_string'] = b'lkmc_exit_status_(\d+)'
consts['baremetal_lib_basename'] = 'lib'
consts['emulator_userland_only_short_to_long_dict'] = collections.OrderedDict([
    ('n', 'native'),
])
consts['all_userland_only_emulators'] = set()
for key in consts['emulator_userland_only_short_to_long_dict']:
    consts['all_userland_only_emulators'].add(key)
    consts['all_userland_only_emulators'].add(consts['emulator_userland_only_short_to_long_dict'][key])
consts['emulator_short_to_long_dict'] = collections.OrderedDict([
    ('q', 'qemu'),
    ('g', 'gem5'),
])
consts['emulator_short_to_long_dict'].update(consts['emulator_userland_only_short_to_long_dict'])
consts['all_long_emulators'] = [consts['emulator_short_to_long_dict'][k] for k in consts['emulator_short_to_long_dict']]
consts['emulator_choices'] = set()
for key in consts['emulator_short_to_long_dict']:
    consts['emulator_choices'].add(key)
    consts['emulator_choices'].add(consts['emulator_short_to_long_dict'][key])
consts['host_arch'] = platform.processor()
consts['guest_lkmc_home'] = os.sep + consts['repo_short_id']
consts['build_type_choices'] = [
    # -O2 -g
    'opt',
    # -O0 -g
    'debug'
]
consts['gem5_build_type_choices'] = consts['build_type_choices'] + [
    'fast', 'prof', 'perf',
]
consts['build_type_default'] = 'opt'
# Files whose basename start with this are gitignored.
consts['tmp_prefix'] = 'tmp.'

class ExitLoop(Exception):
    pass

class LkmcCliFunction(cli_function.CliFunction):
    '''
    Common functionality shared across our CLI functions:

    * command timing
    * a lot some common flags, e.g.: --arch, --dry-run, --quiet, --verbose
    * a lot of helpers that depend on self.env
    +
    self.env contains the command line arguments + a ton of values derived from those.
    +
    It would be beautiful to do this evaluation in a lazy way, e.g. with functions +
    cache decorators:
    https://stackoverflow.com/questions/815110/is-there-a-decorator-to-simply-cache-function-return-values
    '''
    def __init__(
        self,
        *args,
        defaults=None,
        **kwargs
    ):
        '''
        :ptype defaults: Dict[str,Any]
        :param defaults: override the default value of an argument
        '''
        kwargs['default_config_file'] = consts['default_config_file']
        kwargs['extra_config_params'] = os.path.basename(inspect.getfile(self.__class__))
        if defaults is None:
            defaults = {}
        self._defaults = defaults
        self._is_common = True
        self._common_args = set()
        super().__init__(*args, **kwargs)
        self.print_lock = threading.Lock()

        # Args for all scripts.
        arches = consts['arch_short_to_long_dict']
        arches_string = []
        for arch_short in arches:
            arch_long = arches[arch_short]
            arches_string.append('{} ({})'.format(arch_long, arch_short))
        arches_string = ', '.join(arches_string)
        self.add_argument(
            '-A',
            '--all-archs',
            default=False,
            help='''\
Run action for all supported --archs archs. Ignore --archs.
'''.format(arches_string)
        )
        self.add_argument(
            '-a',
            '--arch',
            action='append',
            choices=consts['arch_choices'],
            default=[consts['default_arch']],
            dest='archs',
            help='''\
CPU architecture to use. If given multiple times, run the action
for each arch sequentially in that order. If one of them fails, stop running.
Valid archs: {}
'''.format(arches_string)
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
            '--gcc-which',
            choices=[
                'buildroot',
                'crosstool-ng',
                'host',
                'host-baremetal'
            ],
            default='buildroot',
            help='''\
Which toolchain binaries to use:
- buildroot: the ones we built with ./build-buildroot. For userland, links to glibc.
- crosstool-ng: the ones we built with ./build-crosstool-ng. For baremetal, links to newlib.
- host: the host distro pre-packaged userland ones. For userland, links to glibc.
- host-baremetal: the host distro pre-packaged bare one. For baremetal, links to newlib.
'''
        )
        self.add_argument(
            '--march',
            help='''\
GCC -march option to use. Currently only used for the more LKMC-specific builds such as
./build-userland and ./build-baremetal. Maybe we will use it for more things some day.
'''
        )
        self.add_argument(
            '--mode',
            choices=('userland', 'baremetal'),
            default=None,
            help='''Differentiate between userland and baremetal for scripts that can do both.
./run differentiates between them based on the --userland and --baremetal options,
however those options take arguments, Certain scripts can be run on either user or baremetal mode.
If given, this differentiates between them.
'''
        )
        self.add_argument(
            '-j',
            '--nproc',
            default=len(os.sched_getaffinity(0)),
            type=int,
            help='''Number of processors (Jobs) to use for the action.''',
        )
        self.add_argument(
            '-q',
            '--quiet',
            default=False,
            help='''\
Don't print anything to stdout, except if it is part of an interactive terminal.
TODO: implement fully, some stuff is escaping it currently.
'''
        )
        self.add_argument(
            '--quit-on-fail',
            default=True,
            help='''\
Stop running at the first failed test.
'''
        )
        self.add_argument(
            '--show-cmds',
            default=True,
            help='''\
Print the exact Bash command equivalents being run by this script.
Implied by --quiet.
'''
        )
        self.add_argument(
            '--show-time',
            default=True,
            help='''\
Print how long it took to run the command at the end.
Implied by --quiet.
'''
        )
        self.add_argument(
            '-v',
            '--verbose',
            default=False,
            help='Show full compilation commands when they are not shown by default.'
        )

        # Gem5 args.
        self.add_argument(
            '--dp650', default=False,
            help='Use the ARM DP650 display processor instead of the default HDLCD on gem5.'
        )
        self.add_argument(
            '--gem5-build-dir',
            help='''\
Use the given directory as the gem5 build directory.
Ignore --gem5-build-id and --gem5-build-type.
'''
        )
        self.add_argument(
            '-M',
            '--gem5-build-id',
            help='''\
gem5 build ID. Allows you to keep multiple separate gem5 builds.
Default: {}
'''.format(consts['default_build_id'])
        )
        self.add_argument(
            '--gem5-build-type',
            choices=consts['gem5_build_type_choices'],
            default=consts['build_type_default'],
            help='gem5 build type, most often used for "debug" builds.'
        )
        self.add_argument(
            '--gem5-clang',
            default=False,
            help='''\
Build gem5 with clang and set the --gem5-build-id to 'clang' by default.
'''
        )
        self.add_argument(
            '--gem5-source-dir',
            help='''\
Use the given directory as the gem5 source tree. Ignore `--gem5-worktree`.
'''
        )
        self.add_argument(
            '-N',
            '--gem5-worktree',
            help='''\
Create and use a git worktree of the gem5 submodule.
See: https://cirosantilli.com/linux-kernel-module-cheat#gem5-worktree
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
            '-L',
            '--linux-build-id',
            default=consts['default_build_id'],
            help='''\
Linux build ID. Allows you to keep multiple separate Linux builds.
'''
        )
        self.add_argument(
            '--linux-exec',
            help='''\
Use the given executable Linux kernel image. Ignored in userland and baremetal modes,
Remember that different emulators may take different types of image, see:
https://cirosantilli.com/linux-kernel-module-cheat#vmlinux-vs-bzimage-vs-zimage-vs-image
''',
        )
        self.add_argument(
            '--linux-source-dir',
            help='''\
Use the given directory as the Linux source tree.
'''
        )
        self.add_argument(
            '--initramfs',
            default=False,
            help='''\
See: https://cirosantilli.com/linux-kernel-module-cheat#initramfs
'''
        )
        self.add_argument(
            '--initrd',
            default=False,
            help='''\
For Buildroot: create a CPIO root filessytem.
For QEMU use that CPUI root filesystem initrd instead of the default ext2.
See: https://cirosantilli.com/linux-kernel-module-cheat#initrd
'''
        )

        # Baremetal.
        self.add_argument(
            '-b',
            '--baremetal',
            help='''\
Use the given baremetal executable instead of the Linux kernel.

If the path points to a source code inside baremetal/, then the
corresponding executable is automatically found.
'''
        )

        # Buildroot.
        self.add_argument(
            '--buildroot-build-id',
            default=consts['default_build_id'],
            help='Buildroot build ID. Allows you to keep multiple separate gem5 builds.'
        )
        self.add_argument(
            '--buildroot-linux',
            default=False,
            help='Boot with the Buildroot Linux kernel instead of our custom built one. Mostly for sanity checks.'
        )

        # Android.
        self.add_argument(
            '--rootfs-type',
            default='buildroot',
            choices=('buildroot', 'android'),
            help='Which rootfs to use.'
        )
        self.add_argument(
            '--android-version', default='8.1.0_r60',
            help='Which android version to use. implies --rootfs-type android'
        )
        self.add_argument(
            '--android-base-dir',
            help='''\
If given, place all android sources and build files into the given directory.
One application of this is to put those large directories in your HD instead
of SSD.
'''
        )

        # crosstool-ng
        self.add_argument(
            '--crosstool-ng-build-id',
            default=consts['default_build_id'],
            help='Crosstool-NG build ID. Allows you to keep multiple separate crosstool-NG builds.'
        )
        self.add_argument(
            '--docker',
            default=False,
            help='''\
Use the docker download Ubuntu root filesystem instead of the default Buildroot one.
'''
        )

        # QEMU.
        self.add_argument(
            '--qemu-build-id',
            default=consts['default_build_id'],
            help='QEMU build ID. Allows you to keep multiple separate QEMU builds.'
        )
        self.add_argument(
            '--qemu-build-type',
            choices=consts['build_type_choices'],
            default=consts['build_type_default'],
            help='QEMU build type, most often used for "debug" vs optimized builds.'
        )
        self.add_argument(
            '--qemu-which',
            choices=[consts['repo_short_id'], 'host'],
            default=consts['repo_short_id'],
            help='''\
Which qemu binaries to use: qemu-system-, qemu-, qemu-img, etc.:
- lkmc: the ones we built with ./build-qemu
- host: the host distro pre-packaged provided ones
'''
        )
        self.add_argument(
            '--machine',
            help='''\
Machine type:

* QEMU default: -machine virt
* gem5 default: --machine-type VExpress_GEM5_V1

More infor on platforms at:
https://cirosantilli.com/linux-kernel-module-cheat#gem5-arm-platforms
'''
        )

        # Userland.
        self.add_argument(
            '--copy-overlay',
            default=True,
            help='''\
Copy userland build outputs to the overlay directory which will be put inside
the image. If not given explicitly, this is disabled automatically when certain
options are given, for example --static, since users don't usually want
static executables to be placed in the final image, but rather only for
user mode simulations in simulators that don't support dynamic linking like gem5.
'''
        )
        self.add_argument(
            '--host',
            default=False,
            help='''\
Use the host toolchain and other dependencies to build exectuables for host execution.
Automatically place the build output on a separate directory from non --host builds,
e.g. by defaulting --userland-build-id host if that option has effect for the package.
Make --copy-overlay default to False as the generated executables can't in general
be run in the guest.
''',
        )
        self.add_argument(
            '--out-rootfs-overlay-dir-prefix',
            default='',
            help='''\
Place the output files of userland build outputs inside the image within this
additional prefix. This is mostly useful to place different versions of binaries
with different build parameters inside image to compare them. See:
* https://cirosantilli.com/linux-kernel-module-cheat#update-the-toolchain
* https://cirosantilli.com/linux-kernel-module-cheat#out_rootfs_overlay_dir
'''
        )
        self.add_argument(
            '--package',
            action='append',
            help='''\
Request to install a package in the target root filesystem, or indicate that it is present
when building examples that rely on it or running tests for those examples.
''',
        )
        self.add_argument(
            '--package-all',
            action='store_true',
            help='''\
Indicate that all packages used by our userland/ examples with --package
are available.
''',
        )
        self.add_argument(
            '--static',
            default=False,
            help='''\
Build userland executables statically. Set --userland-build-id to 'static'
if one was not given explicitly. See also:
https://cirosantilli.com/linux-kernel-module-cheat#user-mode-static-executables
''',
        )
        self.add_argument(
            '-u',
            '--userland',
            help='''\
Run the given userland executable in user mode instead of booting the Linux kernel
in full system mode. In gem5, user mode is called Syscall Emulation (SE) mode and
uses se.py.
Path resolution is similar to --baremetal.
'''
        )
        self.add_argument(
            '--userland-args',
            help='''\
CLI arguments to pass to the userland executable.
'''
        )
        self.add_argument(
            '--userland-build-id'
        )

        # Run.
        self.add_argument(
            '--background',
            default=False,
            help='''\
Make programs that would take over the terminal such as QEMU full system run on the
background instead.

Currently only implemented for ./run.

Interactive input cannot be given.

Send QEMU serial output to a file instead of the host terminal.

TODO: use a port instead. If only there was a way to redirect a serial to multiple
places, both to a port and a file? We use the file currently to be able to have
any output at all.
https://superuser.com/questions/1373226/how-to-redirect-qemu-serial-output-to-both-a-file-and-the-terminal-or-a-port
'''
        )
        self.add_argument(
            '--in-tree',
            default=False,
            help='''\
Place build output inside source tree to conveniently run it, especially when
building with the host native toolchain.

When running, use in-tree executables instead of out-of-tree ones,
userland/c/hello resolves userland/c/hello.out instead of the out-of-tree one.

Currently only supported by userland scripts such as ./build-userland and
./run --userland.
''',
        )
        self.add_argument(
            '--port-offset',
            type=int,
            help='''\
Increase the ports to be used such as for GDB by an offset to run multiple
instances in parallel. Default: the run ID (-n) if that is an integer, otherwise 0.
'''
        )
        self.add_argument(
            '--prebuilt',
            default=False,
            help='''\
Use prebuilt packaged host utilities as much as possible instead
of the ones we built ourselves. Saves build time, but decreases
the likelihood of incompatibilities.
'''
        )
        self.add_argument(
            '--run-id',
            default='0',
            help='''\
ID for run outputs such as gem5's m5out. Allows you to do multiple runs,
and then inspect separate outputs later in different output directories.
'''
        )

        # Misc.
        emulators = consts['emulator_short_to_long_dict']
        emulators_string = []
        for emulator_short in emulators:
            emulator_long = emulators[emulator_short]
            emulators_string.append('{} ({})'.format(emulator_long, emulator_short))
        emulators_string = ', '.join(emulators_string)
        self.add_argument(
            '--all-emulators', default=False,
            help='''\
Run action for all supported emulators. Ignore --emulator.
'''.format(emulators_string)
        )
        self.add_argument(
            '-e',
            '--emulator',
            action='append',
            choices=consts['emulator_choices'],
            default=['qemu'],
            dest='emulators',
            help='''\
Emulator to use. If given multiple times, semantics are similar to --arch.
Valid emulators: {}

"native" means running natively on host. It is only supported for userland,
and you must have built the program for native running, see:
https://cirosantilli.com/linux-kernel-module-cheat#userland-setup-getting-started-natively
Incompatible archs are skipped.
'''.format(emulators_string)
        )
        self._is_common = False

    def __call__(self, *args, **kwargs):
        '''
        For Python code calls, in addition to base class behaviour:

        * print the CLI equivalent of the call
        * automatically forward common arguments
        '''
        print_cmd = ['./' + self.extra_config_params, LF]
        if 'print_cmd_oneline' in kwargs:
            force_oneline = kwargs['print_cmd_oneline']
            del kwargs['print_cmd_oneline']
        else:
            force_oneline=False
        for line in self.get_cli(**kwargs):
            print_cmd.extend(line)
            print_cmd.append(LF)
        if not ('quiet' in kwargs and kwargs['quiet']):
            shell_helpers.ShellHelpers().print_cmd(
                print_cmd,
                force_oneline=force_oneline
            )
        return super().__call__(**kwargs)

    def _handle_thread_pool_errors(self, my_thread_pool):
        handle_output_result = my_thread_pool.get_handle_output_result()
        if handle_output_result is not None:
            work_function_input, work_function_return, exception = handle_output_result
            if not type(exception) is thread_pool.ThreadPoolExitException:
                print('work_function or handle_output raised unexpectedly:')
                print(thread_pool.ThreadPool.exception_traceback_string(exception), end='')
                print('work_function_input: {}'.format(work_function_input))
                print('work_function_return: {}'.format(work_function_return))
            return 1
        else:
            return 0

    def _init_env(self, env):
        '''
        Update the kwargs from the command line with values derived from them.
        '''
        def join(*paths):
            return os.path.join(*paths)
        if env['emulator'] in env['emulator_short_to_long_dict']:
            env['emulator'] = env['emulator_short_to_long_dict'][env['emulator']]
        if not env['_args_given']['userland_build_id']:
            if env['static']:
                env['userland_build_id'] = 'static'
            elif env['host']:
                env['userland_build_id'] = 'host'
            else:
                env['userland_build_id'] = env['default_build_id']
        if not env['_args_given']['gem5_build_id']:
            if env['_args_given']['gem5_clang']:
                env['gem5_build_id'] = 'clang'
            elif env['_args_given']['gem5_worktree']:
                env['gem5_build_id'] = env['gem5_worktree']
            else:
                env['gem5_build_id'] = consts['default_build_id']
        env['is_arm'] = False
        # Our approach is as follows:
        #
        # * compilers: control maximum arch version emitted explicitly -mcpu
        # +
        # This helps to prevent blowing up simulation unnecessarily.
        # +
        # It does not matter if we miss any perf features for QEMU which is functional,
        # but it could matter for gem5 perf simulations.
        # * assemblers: enable as many features as possible.
        # +
        # Well, if I'm explicitly writing down the instructions, I want
        # my emulator to blow up in peace!
        # * emulators: enable as many features as possible
        # +
        # This is the gem5 default behavior, for QEMU TODO not sure if default,
        # but we select it explicitly with -cpu max.
        # https://habkost.net/posts/2017/03/qemu-cpu-model-probing-story.html
        # +
        # We doe this because QEMU does not add all possible Cortex Axx, there are
        # just too many, and gem5 does not allow selecting lower feature in general.
        if env['arch'] == 'arm':
            env['armv'] = 7
            env['buildroot_toolchain_prefix'] = 'arm-buildroot-linux-gnueabihf'
            env['crosstool_ng_toolchain_prefix'] = 'arm-unknown-eabi'
            env['ubuntu_toolchain_prefix'] = 'arm-linux-gnueabihf'
            env['is_arm'] = True
            if not env['_args_given']['march']:
                env['march'] = 'armv8-a'
        elif env['arch'] == 'aarch64':
            env['armv'] = 8
            env['buildroot_toolchain_prefix'] = 'aarch64-buildroot-linux-gnu'
            env['crosstool_ng_toolchain_prefix'] = 'aarch64-unknown-elf'
            env['ubuntu_toolchain_prefix'] = 'aarch64-linux-gnu'
            env['is_arm'] = True
            if not env['_args_given']['march']:
                env['march'] = 'armv8-a+lse'
        elif env['arch'] == 'x86_64':
            env['crosstool_ng_toolchain_prefix'] = 'x86_64-unknown-elf'
            env['gem5_arch'] = 'X86'
            env['buildroot_toolchain_prefix'] = 'x86_64-buildroot-linux-gnu'
            env['ubuntu_toolchain_prefix'] = 'x86_64-linux-gnu'
            if env['emulator'] == 'gem5':
                if not env['_args_given']['machine']:
                    env['machine'] = 'TODO'
            else:
                if not env['_args_given']['machine']:
                    env['machine'] = 'pc'
        if env['is_arm']:
            env['gem5_arch'] = 'ARM'
            if env['emulator'] == 'gem5':
                if not env['_args_given']['machine']:
                    if env['dp650']:
                        env['machine'] = 'VExpress_GEM5_V1_DPU'
                    else:
                        env['machine'] = 'VExpress_GEM5_V1'
            else:
                if not env['_args_given']['machine']:
                    env['machine'] = 'virt'

        # Buildroot
        env['buildroot_build_dir'] = join(env['buildroot_out_dir'], 'build', env['buildroot_build_id'], env['arch'])
        env['buildroot_download_dir'] = join(env['buildroot_out_dir'], 'download')
        env['buildroot_config_file'] = join(env['buildroot_build_dir'], '.config')
        env['buildroot_build_build_dir'] = join(env['buildroot_build_dir'], 'build')
        env['buildroot_linux_build_dir'] = join(env['buildroot_build_build_dir'], 'linux-custom')
        env['buildroot_vmlinux'] = join(env['buildroot_linux_build_dir'], 'vmlinux')
        env['buildroot_host_dir'] = join(env['buildroot_build_dir'], 'host')
        env['buildroot_host_bin_dir'] = join(env['buildroot_host_dir'], 'usr', 'bin')
        env['buildroot_pkg_config'] = join(env['buildroot_host_bin_dir'], 'pkg-config')
        env['buildroot_images_dir'] = join(env['buildroot_build_dir'], 'images')
        env['buildroot_rootfs_raw_file'] = join(env['buildroot_images_dir'], 'rootfs.ext2')
        env['buildroot_qcow2_file'] = env['buildroot_rootfs_raw_file'] + '.qcow2'
        env['buildroot_cpio'] = join(env['buildroot_images_dir'], 'rootfs.cpio')
        env['staging_dir'] = join(env['out_dir'], 'staging', env['arch'])
        env['buildroot_staging_dir'] = join(env['buildroot_build_dir'], 'staging')
        env['buildroot_target_dir'] = join(env['buildroot_build_dir'], 'target')
        if not env['_args_given']['linux_source_dir']:
            env['linux_source_dir'] = os.path.join(consts['submodules_dir'], 'linux')
        common.extract_vmlinux = os.path.join(env['linux_source_dir'], 'scripts', 'extract-vmlinux')
        env['linux_buildroot_build_dir'] = join(env['buildroot_build_build_dir'], 'linux-custom')

        # QEMU
        env['qemu_build_dir'] = join(
            env['out_dir'],
            'qemu',
            env['qemu_build_id'],
            env['qemu_build_type']
        )
        env['qemu_img_basename'] = 'qemu-img'
        env['qemu_img_executable'] = join(env['qemu_build_dir'], env['qemu_img_basename'])
        if env['userland'] is None:
            env['qemu_executable_basename'] = 'qemu-system-{}'.format(env['arch'])
        else:
            env['qemu_executable_basename'] = 'qemu-{}'.format(env['arch'])
        if env['qemu_which'] == 'host':
            env['qemu_executable'] = env['qemu_executable_basename']
        else:
            if env['userland'] is None:
                env['qemu_executable'] = join(
                    env['qemu_build_dir'],
                    '{}-softmmu'.format(env['arch']),
                    env['qemu_executable_basename']
                )
            else:
                env['qemu_executable'] = join(
                    self.env['qemu_build_dir'],
                    '{}-linux-user'.format(self.env['arch']),
                    env['qemu_executable_basename']
                )

        # gem5
        if not env['_args_given']['gem5_build_dir']:
            env['gem5_build_dir'] = join(env['gem5_out_dir'], env['gem5_build_id'])
        env['gem5_m5term'] = join(env['gem5_build_dir'], 'm5term')
        env['gem5_build_build_dir'] = join(env['gem5_build_dir'], 'build')
        env['gem5_executable_dir'] = join(env['gem5_build_build_dir'], env['gem5_arch'])
        env['gem5_executable_suffix'] = '.{}'.format(env['gem5_build_type'])
        env['gem5_executable'] = self.get_gem5_target_path(env, 'gem5')
        env['gem5_unit_test_target'] = self.get_gem5_target_path(env, 'unittests')
        env['gem5_system_dir'] = join(env['gem5_build_dir'], 'system')
        env['gem5_system_binaries_dir'] = join(env['gem5_system_dir'], 'binaries')
        if self.env['is_arm']:
            if env['arch'] == 'arm':
                gem5_bootloader_basename = 'boot.arm'
            elif env['arch'] == 'aarch64':
                gem5_bootloader_basename = 'boot.arm64'
            env['gem5_bootloader'] = join(env['gem5_system_binaries_dir'], gem5_bootloader_basename)
        else:
            env['gem5_bootloader'] = None

        # gem5 source
        if env['_args_given']['gem5_source_dir']:
            assert os.path.exists(env['gem5_source_dir'])
        else:
            if env['_args_given']['gem5_worktree']:
                env['gem5_source_dir'] = join(env['gem5_non_default_source_root_dir'], env['gem5_worktree'])
            else:
                env['gem5_source_dir'] = env['gem5_default_source_dir']
        env['gem5_m5_source_dir'] = join(env['gem5_source_dir'], 'util', 'm5')
        env['gem5_config_dir'] = join(env['gem5_source_dir'], 'configs')
        env['gem5_se_file'] = join(env['gem5_config_dir'], 'example', 'se.py')
        env['gem5_fs_file'] = join(env['gem5_config_dir'], 'example', 'fs.py')

        # crosstool-ng
        env['crosstool_ng_buildid_dir'] = join(env['crosstool_ng_out_dir'], 'build', env['crosstool_ng_build_id'])
        env['crosstool_ng_install_dir'] = join(env['crosstool_ng_buildid_dir'], 'install', env['arch'])
        env['crosstool_ng_bin_dir'] = join(env['crosstool_ng_install_dir'], 'bin')
        env['crosstool_ng_source_copy_dir'] = join(env['crosstool_ng_buildid_dir'], 'source')
        env['crosstool_ng_config'] = join(env['crosstool_ng_source_copy_dir'], '.config')
        env['crosstool_ng_defconfig'] = join(env['crosstool_ng_source_copy_dir'], 'defconfig')
        env['crosstool_ng_executable'] = join(env['crosstool_ng_source_copy_dir'], 'ct-ng')
        env['crosstool_ng_build_dir'] = join(env['crosstool_ng_buildid_dir'], 'build')
        env['crosstool_ng_download_dir'] = join(env['crosstool_ng_out_dir'], 'download')

        # run
        env['gem5_run_dir'] = join(env['run_dir_base'], 'gem5', env['arch'], str(env['run_id']))
        env['m5out_dir'] = join(env['gem5_run_dir'], 'm5out')
        env['stats_file'] = join(env['m5out_dir'], 'stats.txt')
        env['gem5_trace_txt_file'] = join(env['m5out_dir'], 'trace.txt')
        env['gem5_guest_terminal_file'] = join(env['m5out_dir'], 'system.terminal')
        env['gem5_readfile_file'] = join(env['gem5_run_dir'], 'readfile')
        env['gem5_termout_file'] = join(env['gem5_run_dir'], 'termout.txt')
        env['qemu_run_dir'] = join(env['run_dir_base'], 'qemu', env['arch'], str(env['run_id']))
        env['qemu_termout_file'] = join(env['qemu_run_dir'], 'termout.txt')
        env['qemu_trace_basename'] = 'trace.bin'
        env['qemu_trace_file'] = join(env['qemu_run_dir'], 'trace.bin')
        env['qemu_trace_txt_file'] = join(env['qemu_run_dir'], 'trace.txt')
        env['qemu_rrfile'] = join(env['qemu_run_dir'], 'rrfile')
        env['gem5_out_dir'] = join(env['out_dir'], 'gem5')

        # Ports
        if not env['_args_given']['port_offset']:
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
            if env['background']:
                env['guest_terminal_file'] = env['qemu_background_serial_file']
            else:
                env['guest_terminal_file'] = env['qemu_termout_file']
            env['trace_txt_file'] = env['qemu_trace_txt_file']
        env['run_cmd_file'] = join(env['run_dir'], 'run.sh')

        # Linux kernel.
        if not env['_args_given']['linux_build_dir']:
            env['linux_build_dir'] = join(env['out_dir'], 'linux', env['linux_build_id'], env['arch'])
        env['lkmc_vmlinux'] = join(env['linux_build_dir'], 'vmlinux')
        if env['arch'] == 'arm':
            env['android_arch'] = 'arm'
            env['linux_arch'] = 'arm'
            env['linux_image_prefix'] = join('arch', env['linux_arch'], 'boot', 'zImage')
        elif env['arch'] == 'aarch64':
            env['android_arch'] = 'arm64'
            env['linux_arch'] = 'arm64'
            env['linux_image_prefix'] = join('arch', env['linux_arch'], 'boot', 'Image')
        elif env['arch'] == 'x86_64':
            env['android_arch'] = 'x86_64'
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
        env['linux_config'] = join(env['linux_build_dir'], '.config')
        if env['emulator']== 'gem5':
            env['userland_quit_cmd'] = join(
                env['guest_lkmc_home'],
                'gem5_exit.sh'
            )
        else:
            env['userland_quit_cmd'] = join(
                env['guest_lkmc_home'],
                'linux',
                'poweroff' + env['userland_executable_ext']
            )
        env['ramfs'] = env['initrd'] or env['initramfs']
        if env['ramfs']:
            env['initarg'] = 'rdinit'
        else:
            env['initarg'] = 'init'
        env['quit_init'] = '{}={}'.format(env['initarg'], env['userland_quit_cmd'])

        # Userland
        env['userland_source_arch_arch_dir'] = join(env['userland_source_arch_dir'], env['arch'])
        if env['in_tree']:
            env['userland_build_dir'] = self.env['userland_source_dir']
        else:
            env['userland_build_dir'] = join(env['out_dir'], 'userland', env['userland_build_id'], env['arch'])
        env['package'] = set(env['package'])
        if not env['_args_given']['copy_overlay']:
            if self.env['in_tree'] or self.env['static'] or self.env['host']:
                env['copy_overlay'] = False

        # Kernel modules.
        env['kernel_modules_build_dir'] = join(env['kernel_modules_build_base_dir'], env['arch'])
        env['kernel_modules_build_subdir'] = join(env['kernel_modules_build_dir'], env['kernel_modules_subdir'])
        env['kernel_modules_build_host_dir'] = join(env['kernel_modules_build_base_dir'], 'host')
        env['kernel_modules_build_host_subdir'] = join(env['kernel_modules_build_host_dir'], env['kernel_modules_subdir'])

        # Overlay.
        # https://cirosantilli.com/linux-kernel-module-cheat#buildroot_packages-directory
        env['out_rootfs_overlay_dir'] = join(env['out_dir'], 'rootfs_overlay', env['arch'])
        env['out_rootfs_overlay_lkmc_dir'] = join(env['out_rootfs_overlay_dir'], env['repo_short_id'])
        env['out_rootfs_overlay_bin_dir'] = join(env['out_rootfs_overlay_dir'], 'bin')

        # Baremetal.
        env['baremetal_source_dir'] = join(env['root_dir'], 'baremetal')
        env['baremetal_source_arch_subpath'] = join('arch', env['arch'])
        env['baremetal_source_arch_dir'] = join(env['baremetal_source_dir'], env['baremetal_source_arch_subpath'])
        env['baremetal_source_lib_dir'] = join(env['baremetal_source_dir'], env['baremetal_lib_basename'])
        env['baremetal_link_script'] = os.path.join(env['baremetal_source_dir'], 'link.ld')
        if env['emulator'] == 'gem5':
            env['simulator_name'] = 'gem5'
        else:
            env['simulator_name'] = 'qemu'
        env['baremetal_build_dir'] = join(env['out_dir'], 'baremetal', env['arch'], env['simulator_name'], env['machine'])
        env['baremetal_build_lib_dir'] = join(env['baremetal_build_dir'], env['baremetal_lib_basename'])
        env['baremetal_syscalls_basename_noext'] = 'syscalls'
        env['baremetal_syscalls_src'] = os.path.join(
            env['baremetal_source_lib_dir'],
            env['baremetal_syscalls_basename_noext'] + self.env['c_ext']
        )
        env['baremetal_syscalls_obj'] = os.path.join(
            self.env['baremetal_build_lib_dir'],
            env['baremetal_syscalls_basename_noext'] + self.env['obj_ext']
        )
        env['baremetal_syscalls_asm_src'] = os.path.join(
            self.env['baremetal_source_lib_dir'],
            env['baremetal_syscalls_basename_noext'] + '_asm' + self.env['asm_ext']
        )
        env['baremetal_syscalls_asm_obj'] = os.path.join(
            self.env['baremetal_build_lib_dir'],
            env['baremetal_syscalls_basename_noext'] + '_asm' + self.env['obj_ext']
        )

        # Userland / baremetal common source.
        env['common_basename_noext'] = env['repo_short_id']
        env['common_c'] = os.path.join(
            env['root_dir'],
            env['common_basename_noext'] + env['c_ext']
        )
        env['common_h'] = os.path.join(
            env['root_dir'],
            env['common_basename_noext'] + env['header_ext']
        )
        if env['mode'] == 'baremetal':
            env['build_dir'] = env['baremetal_build_dir']
        else:
            env['build_dir'] = env['userland_build_dir']

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
        if env['baremetal'] is not None:
            env['disk_image'] = None
            env['image'] = self.resolve_baremetal_executable(env['baremetal'])
            source_path_noext = os.path.splitext(join(
                env['root_dir'],
                env['image'][len(env['baremetal_build_dir']) + 1:]
            ))[0]
            env['source_path'] = None
            for ext in env['baremetal_build_in_exts']:
                source_path = source_path_noext + ext
                if os.path.exists(source_path):
                    env['source_path'] = source_path
                    break
        elif env['userland'] is not None:
            env['image'] = self.resolve_userland_executable(env['userland'])
            source_path_noext = os.path.splitext(join(
                env['userland_source_dir'],
                env['image'][len(env['userland_build_dir']) + 1:]
            ))[0]
            env['source_path'] = None
            for ext in env['build_in_exts']:
                source_path = source_path_noext + ext
                if os.path.exists(source_path):
                    env['source_path'] = source_path
                    break
        else:
            if env['emulator'] == 'gem5':
                if not env['_args_given']['linux_exec']:
                    env['image'] = env['vmlinux']
                if env['ramfs']:
                    env['disk_image'] = None
                else:
                    env['disk_image'] = env['rootfs_raw_file']
            else:
                if not env['_args_given']['linux_exec']:
                    env['image'] = env['linux_image']
                env['disk_image'] = env['qcow2_file']
            if env['_args_given']['linux_exec']:
                env['image'] = env['linux_exec']

        # Android
        if not env['_args_given']['android_base_dir']:
            env['android_base_dir'] = join(env['out_dir'], 'android')
        env['android_dir'] = join(env['android_base_dir'], env['android_version'])
        env['android_build_dir'] = join(env['android_dir'], 'out')
        env['repo_path'] = join(env['android_base_dir'], 'repo')
        env['repo_path_base64'] = env['repo_path'] + '.base64'
        env['android_shell_setup'] = '''\
. build/envsetup.sh
lunch aosp_{}-eng
'''.format(self.env['android_arch'])

        # Toolchain.
        if env['baremetal'] and not env['_args_given']['mode']:
            env['mode'] = 'baremetal'
        if not env['_args_given']['gcc_which']:
            if env['mode'] == 'baremetal':
                env['gcc_which'] = 'crosstool-ng'
            elif env['host']:
                env['gcc_which'] = 'host'
        if env['gcc_which'] == 'buildroot':
            env['toolchain_prefix'] = os.path.join(
                env['buildroot_host_bin_dir'],
                env['buildroot_toolchain_prefix']
            )
            env['userland_library_dir'] = env['buildroot_target_dir']
            env['userland_library_redirects'] = [
                'lib',
                'lib64',
                os.path.join('usr', 'lib'),
                os.path.join('usr', 'lib64')
            ]
            env['pkg_config'] = env['buildroot_pkg_config']
        elif env['gcc_which'] == 'crosstool-ng':
            env['toolchain_prefix'] = os.path.join(
                env['crosstool_ng_bin_dir'],
                env['crosstool_ng_toolchain_prefix']
            )
        elif env['gcc_which'] == 'host':
            if env['arch'] == env['host_arch']:
                env['toolchain_prefix'] = ''
            else:
                env['toolchain_prefix'] = env['ubuntu_toolchain_prefix']
            if env['arch'] == env['host_arch']:
                 env['userland_library_dir'] = '/'
            elif env['arch'] == 'x86_64':
                env['userland_library_dir'] = '/usr/x86_64-linux-gnu/'
            elif env['arch'] == 'arm':
                env['userland_library_dir'] = '/usr/arm-linux-gnueabihf'
            elif env['arch'] == 'aarch64':
                env['userland_library_dir'] = '/usr/aarch64-linux-gnu/'
            env['pkg_config'] = 'pkg-config'
            env['userland_library_redirects'] = ['lib']
        elif env['gcc_which'] == 'host-baremetal':
            if env['arch'] == 'arm':
                env['toolchain_prefix'] = 'arm-none-eabi'
            else:
                raise Exception('There is no host baremetal chain for arch: ' + env['arch'])
        else:
            raise Exception('Unknown toolchain: ' + env['gcc_which'])
        if env['toolchain_prefix'] == '':
            env['toolchain_prefix_dash'] = ''
        else:
            env['toolchain_prefix_dash'] = '{}-'.format(env['toolchain_prefix'])
        env['gfortran_path'] = self.get_toolchain_tool('gfortran')
        env['gcc_path'] = self.get_toolchain_tool('gcc')
        env['gxx_path'] = self.get_toolchain_tool('g++')
        env['ld_path'] = self.get_toolchain_tool('ld')
        if env['gcc_which'] == 'host':
            if env['arch'] == 'x86_64':
                env['gdb_path'] = 'gdb'
            else:
                env['gdb_path'] = 'gdb-multiarch'
        else:
            env['gdb_path'] = self.get_toolchain_tool('gdb')

    def add_argument(self, *args, **kwargs):
        '''
        Also handle:

        - modified defaults from child classes.
        - common arguments to forward on Python calls
        '''
        shortname, longname, key, is_option = self.get_key(*args, **kwargs)
        if key in self._defaults:
            kwargs['default'] = self._defaults[key]
        if self._is_common:
            self._common_args.add(key)
        super().add_argument(*args, **kwargs)

    def assert_is_subpath(self, subpath, parents):
        is_subpath = False
        for parent in parents:
            if self.is_subpath(subpath, parent):
                is_subpath = True
        if not is_subpath:
            raise Exception(
                    'Can only accept targets inside:\n{}\nGiven: {}'.format(
                    '\n'.join(parents),
                    subpath
                )
            )

    def get_elf_entry(self, elf_file_path):
        readelf_header = self.sh.check_output([
            self.get_toolchain_tool('readelf'),
            '-h',
            elf_file_path
        ]).decode()
        for line in readelf_header.decode().split('\n'):
            split = line.split()
            if line.startswith('  Entry point address:'):
                addr = line.split()[-1]
                break
        return int(addr, 0)

    @staticmethod
    def get_gem5_target_path(env, name):
        '''
        Get the magic gem5 target path form the meaningful component name.
        '''
        return os.path.join(env['gem5_executable_dir'], name + env['gem5_executable_suffix'])

    def gem5_list_checkpoint_dirs(self):
        '''
        List checkpoint directory, oldest first.
        '''
        prefix_re = re.compile(self.env['gem5_cpt_prefix'])
        files = list(filter(
            lambda x: os.path.isdir(os.path.join(self.env['m5out_dir'], x))
                      and prefix_re.search(x), os.listdir(self.env['m5out_dir'])
        ))
        files.sort(key=lambda x: os.path.getmtime(os.path.join(self.env['m5out_dir'], x)))
        return files

    def get_common_args(self):
        '''
        These are arguments that might be used by more than one script,
        and are all defined in this class instead of in the derived class
        of the script.
        '''
        return {
            key:self.env[key] for key in self._common_args if
            (
                # Args given on command line.
                self.env['_args_given'][key] or
                # Ineritance changed defaults.
                key in self._defaults
            )
        }

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

    def get_toolchain_tool(self, tool):
        return '{}{}'.format(self.env['toolchain_prefix_dash'], tool)

    def github_make_request(
        self,
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
            'https://' + subdomain + '.github.com/repos/' + self.env['github_repo_id'] + path,
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

    def is_arch_supported(self, arch, mode):
        return not (
            mode == 'baremetal' and
            not arch in consts['crosstool_ng_supported_archs']
        )

    def log_error(self, msg):
        with self.print_lock:
            print('error: {}'.format(msg), file=sys.stdout)

    def log_info(self, msg='', flush=False, **kwargs):
        with self.print_lock:
            if not self.env['quiet']:
                print('{}'.format(msg), **kwargs)
            if flush:
                sys.stdout.flush()

    def log_warn(self, msg):
        with self.print_lock:
            print('warning: {}'.format(msg), file=sys.stdout)

    def is_subpath(self, subpath, parent):
        '''
        https://stackoverflow.com/questions/3812849/how-to-check-whether-a-directory-is-a-sub-directory-of-another-directory
        '''
        return os.path.abspath(subpath).startswith(os.path.abspath(parent))

    def main(self, *args, **kwargs):
        '''
        Run timed_main across all selected archs and emulators.

        :return: if any of the timed_mains exits non-zero and non-null,
                 return that. Otherwise, return 0.
        '''
        env = kwargs.copy()
        self.input_args = env.copy()
        env.update(consts)
        real_all_archs = env['all_archs']
        if real_all_archs:
            real_archs = consts['all_long_archs']
        else:
            real_archs = env['archs']
        real_all_emulators = env['all_emulators']
        if real_all_emulators:
            real_emulators = consts['all_long_emulators']
        else:
            real_emulators = env['emulators']
        return_value = 0
        if env['_args_given']['show_cmds']:
            show_cmds = env['show_cmds']
        else:
            show_cmds = not env['quiet']
        self.setup(env)
        try:
            for emulator in real_emulators:
                for arch in real_archs:
                    if arch in env['arch_short_to_long_dict']:
                        arch = env['arch_short_to_long_dict'][arch]
                    if emulator in env['emulator_short_to_long_dict']:
                        emulator = env['emulator_short_to_long_dict'][emulator]
                    if emulator == 'native':
                        if arch != env['host_arch']:
                            if real_all_archs:
                                continue
                            else:
                                raise Exception('native emulator only supported in if target arch == host arch')
                        if env['userland'] is None and not env['mode'] == 'userland':
                            if real_all_emulators:
                                continue
                            else:
                                raise Exception('native emulator only supported in user mode')
                    if self.is_arch_supported(arch, env['mode']):
                        if not env['dry_run']:
                            start_time = time.time()
                        env['arch'] = arch
                        env['archs'] = [arch]
                        env['_args_given']['archs'] = True
                        env['all_archs'] = False
                        env['emulator'] = emulator
                        env['emulators'] = [emulator]
                        env['_args_given']['emulators'] = True
                        env['all_emulators'] = False
                        self.env = env.copy()
                        self._init_env(self.env)
                        self.sh = shell_helpers.ShellHelpers(
                            dry_run=self.env['dry_run'],
                            quiet=(not show_cmds),
                        )
                        self.setup_one()
                        ret = self.timed_main()
                        if not env['dry_run']:
                            end_time = time.time()
                            self.ellapsed_seconds = end_time - start_time
                            self.print_time(self.ellapsed_seconds)
                        if ret is not None and ret != 0:
                            return_value = ret
                            if self.env['quit_on_fail']:
                                raise ExitLoop()
                    elif not real_all_archs:
                        raise Exception('Unsupported arch for this action: ' + arch)
        except ExitLoop:
            pass
        ret = self.teardown()
        if ret is not None and ret != 0:
            return_value = ret
        return return_value

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
    def seconds_to_hms(seconds):
        '''
        Seconds to hour:minute:seconds

        :ptype seconds: float
        :rtype: str

        https://stackoverflow.com/questions/775049/how-do-i-convert-seconds-to-hours-minutes-and-seconds 
        '''
        frac, whole = math.modf(seconds)
        hours, rem = divmod(whole, 3600)
        minutes, seconds = divmod(rem, 60)
        return '{:02}:{:02}:{:02}'.format(int(hours), int(minutes), int(seconds))

    def print_time(self, ellapsed_seconds):
        if self.env['show_time'] and not self.env['quiet']:
            print('time {}'.format(self.seconds_to_hms(ellapsed_seconds)))

    def raw_to_qcow2(self, qemu_which=False, reverse=False):
        if qemu_which == 'host' or not os.path.exists(self.env['qemu_img_executable']):
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

    def resolve_executable(
        self,
        in_path,
        magic_in_dirs,
        magic_out_dir,
        executable_ext
    ):
        '''
        Resolve the path of an userland or baremetal executable.

        If it is in tree, resolve source paths to their corresponding executables.

        If it is out of tree, return the same exact path as input.

        If the input path is a file, add the executable extension automatically.

        Directories map to the directories that would contain executable in that directory.
        '''
        if not self.env['dry_run'] and not os.path.exists(in_path):
            raise Exception('Input path does not exist: ' + in_path)
        if len(magic_in_dirs) > 1:
            relative_subpath = self.env['root_dir']
        else:
            relative_subpath = magic_in_dirs[0]
        for magic_in_dir in magic_in_dirs:
            if self.is_subpath(in_path, magic_in_dir):
                # Abspath needed to remove the trailing `/.` which makes e.g. rmrf fail.
                out = os.path.abspath(os.path.join(
                    magic_out_dir,
                    os.path.relpath(
                        os.path.splitext(in_path)[0],
                        relative_subpath
                    )
                ))
                if os.path.isfile(in_path):
                    out += executable_ext
                return out
        return in_path

    def resolve_targets(self, source_dirs, targets):
        '''
        Resolve userland or baremetal CLI provided targets to final paths.

        Notably converts the toplevel directory into all source directories needed.
        '''
        if not targets:
            targets = source_dirs.copy()
        new_targets = []
        for target in targets:
            for resolved_target in self.toplevel_to_source_dirs(target, source_dirs):
                self.assert_is_subpath(resolved_target, source_dirs)
                new_targets.append(resolved_target)
        return new_targets

    def resolve_baremetal_executable(self, path):
        return self.resolve_executable(
            path,
            [
                self.env['baremetal_source_dir'],
                self.env['userland_source_dir']
            ],
            self.env['baremetal_build_dir'],
            self.env['baremetal_executable_ext'],
        )

    def resolve_userland_executable(self, path):
        return self.resolve_executable(
            path,
            [self.env['userland_source_dir']],
            self.env['userland_build_dir'],
            self.env['userland_executable_ext'],
        )

    def setup(self, env):
        '''
        Similar to setup run before all timed_main are called.

        _init_env has not yet been called, so only primary CLI arguments may be used.
        '''
        pass

    def setup_one(self):
        '''
        Run just before timed_main, after _init_env.
        '''
        pass

    def toplevel_to_source_dirs(self, path, source_dirs):
        path = os.path.abspath(path)
        if path == self.env['root_dir']:
            return source_dirs
        else:
            return [path]

    def timed_main(self):
        '''
        Main action of the derived class.

        Gets run once for every --arch and every --emulator.
        '''
        pass

    def teardown(self) -> Union[None,int]:
        '''
        Similar to setup, but run once after all timed_main are called.

        :return: if not None, the return integer gets used as the exit status of the program.
        '''
        pass

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
        self._build_arguments = {
            '--ccflags': {
                'default': '',
                'help': '''\
Pass the given compiler flags to all languages (C, C++, Fortran, etc.)
''',
            },
            '--force-rebuild': {
                'default': False,
                "help": '''\
Force rebuild even if sources didn't change.
''',
            },
            '--configure': {
                'default': True,
                "help": '''\
Also run the configuration step during build.
''',
            },
            '--optimization-level': {
                'default': '0',
                'help': '''
Use the given GCC -O optimization level.
For some scripts, there are hard technical challenges why it cannot
be implemented, e.g.: https://cirosantilli.com/linux-kernel-module-cheat#kernel-o0
and for others such as gem5 have their custom mechanism:
https://cirosantilli.com/linux-kernel-module-cheat#gem5-debug-build
''',
            }
        }

    def _add_argument(self, argument_name):
        self.add_argument(
            argument_name,
            **self._build_arguments[argument_name]
        )

    def _build_one(
        self,
        in_path,
        out_path,
        build_exts=None,
        cc_flags=None,
        cc_flags_after=None,
        extra_objs=None,
        extra_objs_userland_asm=None,
        extra_objs_lkmc_common=None,
        extra_objs_baremetal_bootloader=None,
        extra_deps=None,
        link=True,
    ):
        '''
        Build one userland or baremetal executable.
        '''
        if cc_flags is None:
            cc_flags = []
        else:
            cc_flags = cc_flags.copy()
        if cc_flags_after is None:
            cc_flags_after = []
        else:
            cc_flags_after = cc_flags_after.copy()
        if extra_deps is None:
            extra_deps = []
        ret = 0
        in_dir, in_basename = os.path.split(in_path)
        in_dir_abs = os.path.abspath(in_dir)
        dirpath_relative_root = in_dir_abs[len(self.env['root_dir']) + 1:]
        dirpath_relative_root_components = dirpath_relative_root.split(os.sep)
        dirpath_relative_root_components_len = len(dirpath_relative_root_components)
        my_path_properties = path_properties.get(os.path.join(
            dirpath_relative_root,
            in_basename
        ))
        if my_path_properties.should_be_built(
            self.env,
            link,
        ):
            if extra_objs is None:
                extra_objs= []
            if link:
                if self.env['mode'] == 'baremetal' or my_path_properties['extra_objs_lkmc_common']:
                    extra_objs.extend(extra_objs_lkmc_common)
                if (
                    self.env['mode'] == 'baremetal' and
                    not my_path_properties['extra_objs_disable_baremetal_bootloader']
                ):
                    extra_objs.extend(extra_objs_baremetal_bootloader)
                if self.env['mode'] == 'userland':
                    cc_flags_after.extend(['-pthread', LF])
            if self.need_rebuild([in_path] + extra_objs + extra_deps, out_path):
                cc_flags.extend(my_path_properties['cc_flags'])
                if self.env['verbose']:
                    cc_flags.extend([
                        '-v', LF,
                    ])
                cc_flags_after.extend(my_path_properties['cc_flags_after'])
                if my_path_properties['cc_pedantic']:
                    cc_flags.extend(['-pedantic', LF])
                if not link:
                    cc_flags.extend(['-c', LF])
                in_ext = os.path.splitext(in_path)[1]
                if in_ext in (self.env['c_ext'], self.env['asm_ext']):
                    cc = self.env['gcc_path']
                    std = my_path_properties['c_std']
                elif in_ext == self.env['cxx_ext']:
                    cc = self.env['gxx_path']
                    std = my_path_properties['cxx_std']
                if self.env['is_arm']:
                    if in_ext == self.env['asm_ext']:
                        cc_flags.extend([
                            '-Xassembler', '-march=all', LF,
                        ])
                    else:
                        cc_flags.extend([
                            '-march={}'.format(self.env['march']), LF,
                        ])
                if dirpath_relative_root_components_len > 0:
                    if dirpath_relative_root_components[0] == 'userland':
                        if dirpath_relative_root_components_len > 1:
                            if dirpath_relative_root_components[1] == 'libs':
                                if dirpath_relative_root_components_len > 1:
                                    if self.env['gcc_which'] == 'host':
                                        eigen_root = '/'
                                    else:
                                        eigen_root = self.env['buildroot_staging_dir']
                                    packages = {
                                        'boost': {
                                            # Header only, no pkg-config package.
                                            'cc_flags': [],
                                            'cc_flags_after': [],
                                        },
                                        'eigen': {
                                            # TODO: was failing with:
                                            # fatal error: Eigen/Dense: No such file or directory as of
                                            # 975ce0723ee3fa1fea1766e6683e2f3acb8558d6
                                            # http://lists.busybox.net/pipermail/buildroot/2018-June/222914.html
                                            'cc_flags': [
                                                '-I',
                                                os.path.join(
                                                    eigen_root,
                                                    'usr',
                                                    'include',
                                                    'eigen3'
                                                ),
                                                LF
                                            ],
                                            # Header only.
                                            'cc_flags_after': [],
                                        },
                                    }
                                    package_key = dirpath_relative_root_components[2]
                                    if package_key in packages:
                                        package = packages[package_key]
                                    else:
                                        package = {}
                                    if 'cc_flags' in package:
                                        cc_flags.extend(package['cc_flags'])
                                    else:
                                        pkg_config_output = self.sh.check_output([
                                            self.env['pkg_config'],
                                            '--cflags',
                                            package_key
                                        ]).decode()
                                        cc_flags.extend(self.sh.shlex_split(pkg_config_output))
                                    if 'cc_flags_after' in package:
                                        cc_flags.extend(package['cc_flags_after'])
                                    else:
                                        pkg_config_output = subprocess.check_output([
                                            self.env['pkg_config'],
                                            '--libs',
                                            package_key
                                        ]).decode()
                                        cc_flags_after.extend(self.sh.shlex_split(pkg_config_output))
                os.makedirs(os.path.dirname(out_path), exist_ok=True)
                ret = self.sh.run_cmd(
                    (
                        [
                            cc, LF,
                        ] +
                        cc_flags +
                        [
                            '-std={}'.format(std), LF,
                            '-o', out_path, LF,
                            in_path, LF,
                        ] +
                        self.sh.add_newlines(extra_objs) +
                        cc_flags_after
                    ),
                    extra_paths=[self.env['ccache_dir']],
                )
        return ret

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

    def need_rebuild(self, srcs, dst):
        if self.env['force_rebuild']:
            return True
        if not os.path.exists(dst):
            return True
        for src in srcs:
            if os.path.getmtime(src) > os.path.getmtime(dst):
                return True
        return False

    def timed_main(self):
        '''
        Parse CLI, and to the build based on it.

        The actual build work is done by do_build in implementing classes.
        '''
        if self.env['clean']:
            return self.clean()
        else:
            return self.build()

TestStatus = enum.Enum('TestStatus', ['PASS', 'FAIL'])

@functools.total_ordering
class TestResult:
    def __init__(
        self,
        test_id: str ='',
        status : TestStatus =TestStatus.PASS,
        ellapsed_seconds : float =0,
        reason : str =''
    ):
        self.test_id = test_id
        self.status = status
        self.ellapsed_seconds = ellapsed_seconds
        self.reason = reason

    def __eq__(self, other):
        return self.test_id == other.test_id

    def __lt__(self, other):
        return self.test_id < other.test_id

    def __str__(self):
        out = [
            self.status.name,
            LkmcCliFunction.seconds_to_hms(self.ellapsed_seconds),
            repr(self.test_id),
        ]
        if self.status is TestStatus.FAIL:
            out.append(repr(self.reason))
        return ' '.join(out)

class TestCliFunction(LkmcCliFunction):
    '''
    Represents a CLI command that runs tests.

    Automates test reporting boilerplate for those commands.
    '''

    base_run_args = {
        'background': True,
        'ctrl_c_host': True,
        'print_cmd_oneline': True,
        'show_cmds': False,
        'show_stdout': False,
        'show_time': False,
    }

    def __init__(self, *args, **kwargs):
        defaults = {
            'quit_on_fail': False,
            'show_time': False,
        }
        if 'defaults' in kwargs:
            defaults.update(kwargs['defaults'])
        kwargs['defaults'] = defaults
        super().__init__(*args, **kwargs)
        self.test_results = queue.Queue()

    def handle_output_function(
        self,
        work_function_input,
        work_function_return,
        work_function_exception
    ):
        if work_function_exception is not None:
            return work_function_exception
        if work_function_return.status != TestStatus.PASS:
            return thread_pool.ThreadPoolExitException()

    def run_test(
        self,
        run_obj,
        run_args=None,
        test_id=None,
        expected_exit_status=None,
        thread_id=0,
    ):
        '''
        This is a setup / run / teardown setup for simple tests that just do a single run.

        More complex tests might need to run the steps separately, e.g. gdb tests
        must run multiple commands: one for the run and one GDB.

        This function is meant to be called from threads. In particular,
        those threads have to cross over archs: the original motivation is to parallelize
        super slow gem5 boot tests. Therefore, we cannot use self.env['arch'] and selv.env['emulator']
        in this function or callees!

        Ideally, we should make this static and pass all arguments to the call... but lazy to refactor.
        I have the feeling I will regret this one day down the line.

        :param run_obj: callable object
        :param run_args: arguments to be passed to the runnable object
        :param test_id: test identifier, to be added in addition to of arch and emulator ids
        :param thread_id: which thread the test is running under
        '''
        if run_obj.is_arch_supported(run_args['archs'][0], run_args.get('mode', None)):
            cur_run_args = {
                'run_id': thread_id,
            }
            cur_run_args.update(self.base_run_args)
            if run_args is not None:
                cur_run_args.update(run_args)
            test_id_string = self.test_setup(run_args, test_id)
            exit_status = run_obj(**cur_run_args)
            return self.test_teardown(
                run_obj,
                exit_status,
                test_id_string,
                expected_exit_status=expected_exit_status
            )

    def test_setup(self, run_args, test_id):
        test_id_string = '{} {}'.format(run_args['emulators'][0], run_args['archs'][0])
        if test_id is not None and str(test_id) != '':
            test_id_string += ' {}'.format(test_id)
        self.log_info('Starting: {}'.format(repr(test_id_string)), flush=True)
        return test_id_string

    def test_teardown(
        self,
        run_obj,
        exit_status,
        test_id_string,
        expected_exit_status=None
    ):
        if expected_exit_status is None:
            expected_exit_status = 0
        reason = ''
        if not self.env['dry_run']:
            if exit_status == expected_exit_status:
                test_status = TestStatus.PASS
            else:
                test_status = TestStatus.FAIL
                reason = 'wrong exit status, got {} expected {}'.format(
                    exit_status,
                    expected_exit_status
                )
            ellapsed_seconds = run_obj.ellapsed_seconds
        else:
            test_status = TestStatus.PASS
            ellapsed_seconds = 0
        test_result = TestResult(
            test_id_string,
            test_status,
            ellapsed_seconds,
            reason
        )
        self.log_info('Result: ' + str(test_result))
        self.test_results.put(test_result)
        return test_result

    def teardown(self):
        '''
        :return: 1 if any test failed, 0 otherwise
        '''
        self.log_info('\nTest result summary:')
        passes = []
        fails = []
        while not self.test_results.empty():
            test = self.test_results.get()
            if test.status in (TestStatus.PASS, None):
                bisect.insort(passes, test)
            else:
                bisect.insort(fails, test)
        for test in itertools.chain(passes, fails):
            self.log_info(test)
        if fails:
            self.log_error('A test failed')
            return 1
        return 0

# IO format.

class LkmcList(list):
    '''
    list with a lightweight serialization format for algorithm IO.
    '''
    def __init__(self, *args, **kwargs):
        if 'oneline' in kwargs:
            self.oneline = kwargs['oneline']
            del kwargs['oneline']
        else:
            self.oneline = False
        super().__init__(*args, **kwargs)
    def __str__(self):
        if self.oneline:
            sep = ' '
        else:
            sep = '\n'
        return sep.join([str(item) for item in self])

class LkmcOrderedDict(collections.OrderedDict):
    '''
    dict with a lightweight serialization format for algorithm IO.
    '''
    def __str__(self):
        out = []
        for key in self:
            out.extend([
                str(key),
                str(self[key]) + '\n',
            ])
        return '\n'.join(out)
