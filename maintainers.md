# Maintainers

## How to update the Linux kernel?

    # Last point before out patches.
    last_mainline_revision=v4.14
    next_mainline_revision=v4.15
    cd linux

    # Create a branch before the rebase.
    git branch "lkmc-${last_mainline_revision}"
    git remote set-url origin git@github.com:cirosantilli/linux.git
    git push

    git remote add up git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
    git fetch up
    git rebase --onto "$next_mainline_revision" "$last_mainline_revision"
    ./build -t linux-reconfigure
    # Manually fix our kernel modules if necessary.

    cd ..
    git branch "buildroot-2017.08-linux-${last_mainline_revision}"
    git add .
    git commit -m "Linux ${next_mainline_revision}"
    git push

and update the README!

During update all you kernel modules may break since the kernel API is not stable.

They are usually trivial breaks of things moving around headers or to sub-structs.

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
