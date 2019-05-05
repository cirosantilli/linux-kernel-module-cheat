#!/usr/bin/env python3

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
    ):
        self.exit_status = exit_status
        self.interactive = interactive
        self.more_than_1s = more_than_1s

    def should_be_tested(self):
        return \
            not self.interactive and \
            not self.more_than_1s

executable_properties = {
    'c/assert_fail.c': ExecutableProperties(exit_status=1),
    'c/false.c': ExecutableProperties(exit_status=1),
    'c/infinite_loop.c': ExecutableProperties(more_than_1s=True),
    'posix/count.c': ExecutableProperties(more_than_1s=True),
    'posix/sleep_forever.c': ExecutableProperties(more_than_1s=True),
    'posix/virt_to_phys_test.c': ExecutableProperties(more_than_1s=True),
}

def get(test_path):
    if test_path in executable_properties:
        return executable_properties[test_path]
    else:
        return ExecutableProperties()
