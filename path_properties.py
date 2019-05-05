#!/usr/bin/env python3

import os

class PathProperties:
    '''
    Encodes properties of userland and baremetal paths.
    For directories, it applies to all files under the directory.
    Used to determine how to build and test the examples.
    '''
    def __init__(
        self,
        **kwargs
    ):
        self.properties = {
            'allowed_archs': None,
            'exit_status': 0,
            'interactive': False,
            'more_than_1s': False,
            # The path does not generate an executable in itself, e.g.
            # it only generates intermediate object files.
            'no_executable': False,
            # the test receives a signal. We skip those tests for now,
            # on userland because we are lazy to figure out the exact semantics
            # of how Python + QEMU + gem5 determine the exit status of signals.
            'receives_signal': False,
            'requires_kernel_modules': False,
        }
        for key in kwargs:
            if not key in self.properties:
                raise ValueError('Unknown key: {}'.format(key))
        self.properties.update(kwargs)

    def __getitem__(self, key):
        return self.properties[key]

    def update(self, other):
        return self.properties.update(other.properties)

    def should_be_tested(self, arch):
        return \
            not self['interactive'] and \
            not self['more_than_1s'] and \
            not self['no_executable'] and \
            not self['receives_signal'] and \
            not self['requires_kernel_modules'] and \
            (
                self['allowed_archs'] is None or
                arch in self['allowed_archs']
            )

class PrefixTree:
    def __init__(self, children=None, value=None):
        if children == None:
            children = {}
        self.children = children
        self.value = value

path_properties_tree = PrefixTree({
    'arch': PrefixTree({
        'x86_64': PrefixTree(
            {
                'c': PrefixTree({
                    'ring0.c': PrefixTree(value=PathProperties(receives_signal=True))
                })
            },
            PathProperties(allowed_archs={'x86_64'}),
        ),
        'arm': PrefixTree(value=PathProperties(allowed_archs={'arm'})),
        'aarch64': PrefixTree(value=PathProperties(allowed_archs={'aarch64'})),
        'empty.S': PrefixTree(value=PathProperties(no_executable=True)),
        'fail.S': PrefixTree(value=PathProperties(no_executable=True)),
        'main.c': PrefixTree(value=PathProperties(no_executable=True)),
    }),
    'c': PrefixTree({
        'assert_fail.c': PrefixTree(value=PathProperties(exit_status=1)),
        'false.c': PrefixTree(value=PathProperties(exit_status=1)),
        'infinite_loop.c': PrefixTree(value=PathProperties(more_than_1s=True)),
    }),
    'kernel_modules': PrefixTree(value=PathProperties(requires_kernel_modules=True)),
    'linux': PrefixTree(value=PathProperties(requires_kernel_modules=True)),
    'posix': PrefixTree({
        'count.c': PrefixTree(value=PathProperties(more_than_1s=True)),
        'sleep_forever.c': PrefixTree(value=PathProperties(more_than_1s=True)),
        'virt_to_phys_test.c': PrefixTree(value=PathProperties(more_than_1s=True)),
    })
})

def get(test_path):
    cur_node = path_properties_tree
    path_properties = PathProperties()
    for path_component in test_path.split(os.sep):
        if path_component in cur_node.children:
            cur_node = cur_node.children[path_component]
            if cur_node.value is not None:
                path_properties.update(cur_node.value)
        else:
            break
    return path_properties
