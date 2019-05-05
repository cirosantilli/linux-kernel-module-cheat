#!/usr/bin/env python3

import os

class ExecutableProperties:
    '''
    Encodes properties of userland and baremetal examples.
    For directories, it applies to all files under the directory.
    Used to determine how to build and test the examples.
    '''
    def __init__(
        self,
        exit_status=0,
        interactive=False,
        more_than_1s=False,
        receives_signal=False,
        requires_kernel_modules=False,
    ):
        '''
        :param receives_signal: the test receives a signal. We skip those tests for now,
                on userland because we are lazy to figure out the exact semantics
                of how Python + QEMU + gem5 determine the exit status of signals.
        '''
        self.exit_status = exit_status
        self.interactive = interactive
        self.more_than_1s = more_than_1s
        self.receives_signal = receives_signal
        self.requires_kernel_modules = requires_kernel_modules

    def should_be_tested(self):
        return \
            not self.interactive and \
            not self.more_than_1s and \
            not self.receives_signal

executable_properties = {
    'arch/x86_64/c/ring0.c': ExecutableProperties(receives_signal=True),
    'c/assert_fail.c': ExecutableProperties(exit_status=1),
    'c/false.c': ExecutableProperties(exit_status=1),
    'c/infinite_loop.c': ExecutableProperties(more_than_1s=True),
    'kernel_modules': ExecutableProperties(requires_kernel_modules=True),
    'posix/count.c': ExecutableProperties(more_than_1s=True),
    'posix/sleep_forever.c': ExecutableProperties(more_than_1s=True),
    'posix/virt_to_phys_test.c': ExecutableProperties(more_than_1s=True),
}

def get(test_path):
    test_path_components = test_path.split(os.sep)
    if test_path in executable_properties:
        return executable_properties[test_path]
    else:
        return ExecutableProperties()
