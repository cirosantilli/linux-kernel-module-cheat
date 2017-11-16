# Maintainers

## How to add new Buildroot options?

    cd buildroot/output.x86_64~
    make menuconfig

Hit `/` and search for the settings.

Save and quit.

    diff .config.olg .config

Copy and paste the diff additions to `buildroot_config_fragment`.
