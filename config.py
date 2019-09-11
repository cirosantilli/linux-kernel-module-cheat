'''
https://cirosantilli.com/linux-kernel-module-cheat#default-command-line-arguments
'''

def set_args(args, script_name):
    # archs in plural here because --arch adds items to a list of archs.
    args['archs'] = ['aarch64']
    args['emulators'] = ['gem5']
    if script_name == 'build-gem5':
        # This argument is defined only for ./build-gem5.
        args['extra_scons_args'] = ['ADSF=qwer']
