# init

## What is the init executable?

When the Linux kernel finishes booting, it runs an executable as the first and only userland process.

The default path is `/init`, but we an set a custom one with the `init=` kernel command line argument.

This process is then responsible for setting up the entire userland (or destroying everything when you want to have fun).

This typically means reading some configuration files (e.g. `/etc/initrc`) and forking a bunch of userland executables based on those files.

systemd is a "popular" `/init` implementation for desktop distros as of 2017.

BusyBox provides its own minimalistic init implementation which Buildroot uses by default.

### Custom init

Is the default BusyBox `/init` too bloated for you, minimalism freak?

No problem, just use the `init` kernel boot parameter:

    ./runqemu -e 'init=/sleep_forever.out'

Remember that shell scripts can also be used for `init` <https://unix.stackexchange.com/questions/174062/init-as-a-shell-script/395375#395375>:

    ./runqemu -e 'init=/count.sh'

Also remember that if your init returns, the kernel will panic, there are just two non-panic possibilities:

- run forever in a loop or long sleep
- `poweroff` the machine

### BusyBox init is fine, but networking timeout gets on my nerves

I know, right?

Add this line to `rootfs_post_build_script`:

    rm -f "${1}/etc/init.d/"S*network

To restore it, run:

    ./run -t initscripts-reconfigure
