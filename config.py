def set_args(args, script_name):
    args['arch'] = 'aarch64'
    args['gem5'] = True
    if script_name == 'build-gem5':
        # This argument is defined only for ./build-gem5.
        args['extra_scons_args'] = ['ADSF=qwer']
