#!/usr/bin/env python3

import argparse
import imp
import os
import sys

class _Argument:
    def __init__(
            self,
            long_or_short_1,
            long_or_short_2=None,
            default=None,
            help=None,
            nargs=None,
            **kwargs
        ):
            self.args = []
            # argparse is crappy and cannot tell us if arguments were given or not.
            # We need that information to decide if the config file should override argparse or not.
            # So we just use None as a sentinel.
            self.kwargs = {'default': None}
            shortname, longname, key, is_option = self.get_key(
                long_or_short_1,
                long_or_short_2
            )
            if shortname is not None:
                self.args.append(shortname)
            if is_option:
                self.args.append(longname)
            else:
                self.args.append(key)
                self.kwargs['metavar'] = longname
                if default is not None and nargs is None:
                    self.kwargs['nargs'] = '?'
            if nargs is not None:
                self.kwargs['nargs'] = nargs
            if default is True:
                bool_action = 'store_false'
                self.is_bool = True
            elif default is False:
                bool_action = 'store_true'
                self.is_bool = True
            else:
                self.is_bool = False
                if default is None and nargs in ('*', '+'):
                    default = []
            if self.is_bool and not 'action' in kwargs:
                self.kwargs['action'] = bool_action
            if help is not None:
                if default is not None:
                    if help[-1] == '\n':
                        if '\n\n' in help[:-1]:
                            help += '\n'
                    elif help[-1] == ' ':
                        pass
                    else:
                        help += ' '
                    help += 'Default: {}'.format(default)
                self.kwargs['help'] = help
            self.optional = (
                default is not None or
                self.is_bool or
                is_option or
                nargs in ('?', '*', '+')
            )
            self.kwargs.update(kwargs)
            self.default = default
            self.longname = longname
            self.key = key
            self.is_option = is_option

    def __str__(self):
        return str(self.args) + ' ' + str(self.kwargs)

    @staticmethod
    def get_key(
        long_or_short_1,
        long_or_short_2=None,
        **kwargs
    ):
        if long_or_short_2 is None:
            shortname = None
            longname = long_or_short_1
        else:
            shortname = long_or_short_1
            longname = long_or_short_2
        if longname[0] == '-':
            key = longname.lstrip('-').replace('-', '_')
            is_option = True
        else:
            key = longname.replace('-', '_')
            is_option = False
        return shortname, longname, key, is_option

class CliFunction:
    '''
    Represent a function that can be called either from Python code, or
    from the command line.

    Features:

    * single argument description in format very similar to argparse
    * handle default arguments transparently in both cases
    * expose a configuration file mechanism to get default parameters from a file
    * fix some argparse.ArgumentParser() annoyances:
    ** allow dashes in positional arguments:
       https://stackoverflow.com/questions/12834785/having-options-in-argparse-with-a-dash
    ** boolean defaults automatically use store_true or store_false, and add a --no-* CLI
       option to invert them if set from the config

    This somewhat duplicates: https://click.palletsprojects.com but:

    * that decorator API is insane
    * CLI + Python for single functions was wontfixed: https://github.com/pallets/click/issues/40
    '''
    def __call__(self, **args):
        '''
        Python version of the function call.

        :type arguments: Dict
        '''
        args_with_defaults = args.copy()
        # Add missing args from config file.
        if 'config_file' in args_with_defaults and args_with_defaults['config_file'] is not None:
            config_file = args_with_defaults['config_file']
        else:
            config_file = self._config_file
        if os.path.exists(config_file):
            config_configs = {}
            config = imp.load_source('config', config_file)
            config.set_args(config_configs)
            for key in config_configs:
                if key not in self._all_keys:
                    raise Exception('Unknown key in config file: ' + key)
                if (not key in args_with_defaults) or args_with_defaults[key] is None:
                    args_with_defaults[key] = config_configs[key]
        # Add missing args from hard-coded defaults.
        for argument in self._arguments:
            key = argument.key
            if (not key in args_with_defaults) or args_with_defaults[key] is None:
                if argument.optional:
                    args_with_defaults[key] = argument.default
                else:
                    raise Exception('Value not given for mandatory argument: ' + key)
        return self.main(**args_with_defaults)

    def __init__(self, config_file=None, description=None):
        self._all_keys = set()
        self._arguments = []
        self._config_file = config_file
        self._description = description
        if self._config_file is not None:
            self.add_argument(
                '--config-file',
                default=self._config_file,
                help='Path to the configuration file to use'
            )

    def __str__(self):
        return '\n'.join(str(arg) for arg in self._arguments)

    def add_argument(
            self,
            *args,
            **kwargs
        ):
            argument = _Argument(*args, **kwargs)
            self._arguments.append(argument)
            self._all_keys.add(argument.key)

    def cli(self, cli_args=None):
        '''
        Call the function from the CLI. Parse command line arguments
        to get all arguments.
        '''
        parser = argparse.ArgumentParser(
            description=self._description,
            formatter_class=argparse.RawTextHelpFormatter,
        )
        for argument in self._arguments:
            parser.add_argument(*argument.args, **argument.kwargs)
            if argument.is_bool:
                new_longname = '--no' + argument.longname[1:]
                kwargs = argument.kwargs.copy()
                kwargs['default'] = not argument.default
                if kwargs['action'] == 'store_false':
                    kwargs['action'] = 'store_true'
                elif kwargs['action'] == 'store_true':
                    kwargs['action'] = 'store_false'
                if 'help' in kwargs:
                    del kwargs['help']
                parser.add_argument(new_longname, dest=argument.key, **kwargs)
        args = parser.parse_args(args=cli_args)
        return self(**vars(args))

    def cli_exit(self, *args, **kwargs):
        '''
        Same as cli, but also exit the program with int(cli().
        '''
        sys.exit(int(self.cli(*args, **kwargs)))

    @staticmethod
    def get_key(*args, **kwargs):
        return _Argument.get_key(*args, **kwargs)

    def main(self, **kwargs):
        '''
        Do the main function call work.

        :type arguments: Dict
        '''
        raise NotImplementedError

if __name__ == '__main__':
    class OneCliFunction(CliFunction):
        def __init__(self):
            super().__init__(
                config_file='cli_function_test_config.py',
                description = '''\
Description of this
amazing function!
''',
            )
            self.add_argument('-a', '--asdf', default='A', help='Help for asdf'),
            self.add_argument('-q', '--qwer', default='Q', help='Help for qwer'),
            self.add_argument('-b', '--bool', default=True, help='Help for bool'),
            self.add_argument('--bool-cli', default=False, help='Help for bool'),
            self.add_argument('--bool-nargs', default=False, nargs='?', action='store', const='')
            self.add_argument('--no-default', help='Help for no-bool'),
            self.add_argument('pos-mandatory', help='Help for pos-mandatory', type=int),
            self.add_argument('pos-optional', default=0, help='Help for pos-optional', type=int),
            self.add_argument('args-star', help='Help for args-star', nargs='*'),
        def main(self, **kwargs):
            del kwargs['config_file']
            return kwargs

    one_cli_function = OneCliFunction()

    # Default code call.
    default = one_cli_function(pos_mandatory=1)
    assert default == {
        'asdf': 'A',
        'qwer': 'Q',
        'bool': True,
        'bool_nargs': False,
        'bool_cli': True,
        'no_default': None,
        'pos_mandatory': 1,
        'pos_optional': 0,
        'args_star': []
    }

    # Default CLI call with programmatic CLI arguments.
    out = one_cli_function.cli(['1'])
    assert out == default

    # asdf
    out = one_cli_function(pos_mandatory=1, asdf='B')
    assert out['asdf'] == 'B'
    out['asdf'] = default['asdf']
    assert(out == default)

    # asdf and qwer
    out = one_cli_function(pos_mandatory=1, asdf='B', qwer='R')
    assert out['asdf'] == 'B'
    assert out['qwer'] == 'R'
    out['asdf'] = default['asdf']
    out['qwer'] = default['qwer']
    assert(out == default)

    if '--bool':
        out = one_cli_function(pos_mandatory=1, bool=False)
        cli_out = one_cli_function.cli(['--bool', '1'])
        assert out == cli_out
        assert out['bool'] == False
        out['bool'] = default['bool']
        assert(out == default)

    if '--bool-nargs':

        out = one_cli_function(pos_mandatory=1, bool_nargs=True)
        assert out['bool_nargs'] == True
        out['bool_nargs'] = default['bool_nargs']
        assert(out == default)

        out = one_cli_function(pos_mandatory=1, bool_nargs='asdf')
        assert out['bool_nargs'] == 'asdf'
        out['bool_nargs'] = default['bool_nargs']
        assert(out == default)

    # Force a boolean value set on the config to be False on CLI.
    assert one_cli_function.cli(['--no-bool-cli', '1'])['bool_cli'] is False

    # CLI call with argv command line arguments.
    print(one_cli_function.cli())
