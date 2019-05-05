#!/usr/bin/env python3

class ExampleProperties:
    '''
    Encodes properties of userland and baremetal examples.
    For directories, it applies to all files under the directory.
    Used to determine how to build and test the examples.
    '''
    def __init__(
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
    'userland/arch/x86_64/c/ring0.c': ExecutableProperties(exits_nonzero=True),
}
