# Maintainers

## How to update the Linux kernel version?

TODO I can only do it "easily" if the kernel headers are supported, check `buildroot/package/linux-headers/Config.in.host` to see if `BR2_PACKAGE_HOST_LINUX_HEADERS_CUSTOM_4_12` is there. Once you've found a supported version:

    cd linux
    git rebase --onto v4.12 v4.9.6
    # git rebase --onto <new> <old base with our patches on top>

And on `buildroot_config_fragment` edit:

    BR2_LINUX_KERNEL_CUSTOM_VERSION_VALUE="4.12"
    BR2_PACKAGE_HOST_LINUX_HEADERS_CUSTOM_4_12=y

## How to add new Buildroot options?

    cd buildroot/output.x86_64~
    make menuconfig

Hit `/` and search for the settings.

Save and quit.

    diff .config.olg .config

Copy and paste the diff additions to `buildroot_config_fragment`.
