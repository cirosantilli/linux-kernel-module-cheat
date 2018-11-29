#!/usr/bin/env python3

import argparse
import imp
import os

class Argument:
    def __init__(
            self,
            longname,
            shortname=None,
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
            if shortname is not None:
                self.args.append(shortname)
            if longname[0] == '-':
                self.args.append(longname)
                self.key = longname.lstrip('-').replace('-', '_')
            else:
                self.key = longname.replace('-', '_')
                self.args.append(self.key)
                self.kwargs['metavar'] = longname
                if default is not None and nargs is None:
                    self.kwargs['nargs'] = '?'
            if nargs is not None:
                self.kwargs['nargs'] = nargs
            if default is True:
                self.kwargs['action'] = 'store_false'
                self.is_bool = True
            elif default is False:
                self.kwargs['action'] = 'store_true'
                self.is_bool = True
            else:
                self.is_bool = False
                if default is None and nargs in ('*', '+'):
                    default = []
            if help is not None:
                if not self.is_bool and default:
                    help += ' Default: {}'.format(default)
                self.kwargs['help'] = help
            self.optional = (
                default is not None or
                self.is_bool or
                nargs in ('?', '*', '+')
            )
            self.kwargs.update(kwargs)
            self.default = default

    def __str__(self):
        return str(self.args) + ' ' + str(self.kwargs)

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
    ** boolean defaults automatically use store_true or store_false

    This somewhat duplicates: https://click.palletsprojects.com but:

    * that decorator API is insane
    * CLI + Python for single functions was wontfixed: https://github.com/pallets/click/issues/40
    '''
    def __call__(self, **args):
        '''
        Python version of the function call.

        :type arguments: Dict
        '''
        arguments = self._get_arguments()
        args_with_defaults = args.copy()
        # Add missing args from config file.
        if 'config_file' in args_with_defaults and args_with_defaults['config_file'] is not None:
            config_file = args_with_defaults['config_file']
        else:
            config_file = self.default_config
        if os.path.exists(config_file):
            all_keys = {argument.key for argument in arguments}
            config_configs = {}
            config = imp.load_source('config', config_file)
            config.set_args(config_configs)
            for key in config_configs:
                if key not in all_keys:
                    raise Exception('Unknown key in config file: ' + key)
                if (not key in args_with_defaults) or args_with_defaults[key] is None:
                    args_with_defaults[key] = config_configs[key]
        # Add missing args from hard-coded defaults.
        for argument in arguments:
            key = argument.key
            if (not key in args_with_defaults) or args_with_defaults[key] is None:
                if argument.optional:
                    args_with_defaults[key] = argument.default
                else:
                    raise Exception('Value not given for mandatory argument: ' + key)
        return self.main(**args_with_defaults)

    def __init__(self):
        self.default_config = 'config.py'

    def _get_arguments(self):
        '''
        Define the arguments that the function takes.

        :rtype: List[Argument]
        '''
        args = self.get_arguments()
        config_file = self.get_config_file()
        if config_file is not None:
            args.append(Argument(
                longname='--config-file',
                default=config_file,
                help='Path to the configuration file to use'
            ))
        return args

    def cli(self, args=None):
        '''
        Call the function from the CLI. Parse command line arguments
        to get all arguments.
        '''
        parser = argparse.ArgumentParser(
            description=self.get_description(),
            formatter_class=argparse.RawTextHelpFormatter,
        )
        for argument in self._get_arguments():
            parser.add_argument(*argument.args, **argument.kwargs)
        args = parser.parse_args(args=args)
        return self(**vars(args))

    def get_arguments(self):
        '''
        Define the arguments that the function takes.

        :rtype: List[Argument]
        '''
        raise NotImplementedError

    def get_config_file(self):
        '''
        :rtype: Union[None,str]
        '''
        return None

    def get_description(self):
        '''
        argparse.ArgumentParser() description.

        :rtype: Union[Any,str]
        '''
        return None

    def main(self, arguments):
        '''
        Do the main function call work.

        :type arguments: Dict
        '''
        raise NotImplementedError

if __name__ == '__main__':
    class OneCliFunction(CliFunction):
        def get_arguments(self):
            return [
                Argument(shortname='-a', longname='--asdf', default='A', help='Help for asdf'),
                Argument(shortname='-q', longname='--qwer', default='Q', help='Help for qwer'),
                Argument(shortname='-b', longname='--bool', default=True, help='Help for bool'),
                Argument(longname='pos-mandatory', help='Help for pos-mandatory', type=int),
                Argument(longname='pos-optional', default=0, help='Help for pos-optional', type=int),
                Argument(longname='args-star', help='Help for args-star', nargs='*'),
            ]
        def main(self, **kwargs):
            return \
                kwargs['asdf'], \
                kwargs['qwer'], \
                kwargs['bool'], \
                kwargs['pos_optional'], \
                kwargs['pos_mandatory'], \
                kwargs['args_star']
        def get_config_file(self):
            return 'test_config.py'
        def get_description(self):
            return '''\
Description of this
amazing function!
'''

    # Code calls.
    assert OneCliFunction()(pos_mandatory=1) == ('A', 'Q', True, 0, 1, [])
    assert OneCliFunction()(pos_mandatory=1, asdf='B') == ('B', 'Q', True, 0, 1, [])
    assert OneCliFunction()(pos_mandatory=1, bool=False) == ('A', 'Q', False, 0, 1, [])
    assert OneCliFunction()(pos_mandatory=1, asdf='B', qwer='R', bool=False) == ('B', 'R', False, 0, 1, [])

    # CLI call.
    print(OneCliFunction().cli())
