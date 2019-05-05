#!/usr/bin/env python3

class FileProperties:
    def __init__(
        more_than_1s=False,
        exits_nonzero=False,
        interactive=False,
    ):
        self.more_than_1s = more_than_1s
        self.exits_nonzero = exits_nonzero

executable_properties = {
    'userland/arch/x86_64/c/ring0.c': ExecutableProperties(exits_nonzero=True),
}
