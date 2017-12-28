# Maintainers

## How to update the Linux kernel?

If you don't care about educational patches:

    cd linux
    git fetch
    git checkout master

If you do:

    last_mainline_revision=v4.14
    git rebase --onto master $last_mainline_revision

Then rebuild the kernel:

    ./build -t linux-reconfigure

Now, all you kernel modules may break, although they are usually trivial breaks of things moving around headers or to sub-structs.

The userland, however, should simply not break, as Linus enforces strict backwards compatibility of userland interfaces.

This backwards compatibility is just awesome, it makes getting and running the latest master painless.

This also makes this repo the perfect setup to develop the Linux kernel.

## How to downgrade the Linux kernel?

The kernel is not forward compatible, however, so downgrading the Linux kernel requires downgrading the userland too to the latest Buildroot branch that supports it.

The default Linux kernel version is bumped in Buildroot with commit messages of type:

    linux: bump default to version 4.9.6

So you can try:

    git log --grep 'linux: bump default to version'

Those commits change `BR2_LINUX_KERNEL_LATEST_VERSION` in `/linux/Config.in`.

You should then look up if there is a branch that supports that kernel. Staying on branches is a good idea as they will get backports, in particular ones that fix the build as newer host versions come out.

## How to add new Buildroot options?

    cd buildroot/output.x86_64~
    make menuconfig

Hit `/` and search for the settings.

Save and quit.

    diff .config.olg .config

Copy and paste the diff additions to `buildroot_config_fragment`.
