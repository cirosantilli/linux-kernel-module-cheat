# init

## What is the init executable?

When the Linux kernel finishes booting, it runs an executable as the first and only userland process.

The default path is `/init`, but we an set a custom one with the `init=` kernel command line argument.

This process is then responsible for setting up the entire userland (or destroying everything when you want to have fun).

This typically means reading some configuration files (e.g. `/etc/initrc`) and forking a bunch of userland executables based on those files.

systemd is a "popular" `/init` implementation for desktop distros as of 2017.

BusyBox provides its own minimalistic init implementation which Buildroot uses by default.

## Custom init

Is the default BusyBox `/init` too bloated for you, minimalism freak?

No problem, just use the `init` kernel boot parameter:

    ./run -e 'init=/sleep_forever.out'

Remember that shell scripts can also be used for `init` <https://unix.stackexchange.com/questions/174062/init-as-a-shell-script/395375#395375>:

    ./run -e 'init=/count.sh'

Also remember that if your init returns, the kernel will panic, there are just two non-panic possibilities:

- run forever in a loop or long sleep
- `poweroff` the machine

## Disable networking

The default BusyBox init scripts enable networking, and there is a 15 second timeout in case your network is down or if your kernel / emulator setup does not support it.

To disable networking, use:

    ./build -p -n

To restore it, run:

    ./build -t initscripts-reconfigure

## The init environment

The docs make it clear https://www.kernel.org/doc/html/v4.14/admin-guide/kernel-parameters.html

> The kernel parses parameters from the kernel command line up to “–”;
> if it doesn’t recognize a parameter and it doesn’t contain a ‘.’, the parameter gets passed to init:
> parameters with ‘=’ go into init’s environment, others are passed as command line arguments to init.
> Everything after “–” is passed as an argument to init.

And you can try it out with:

    ./run -e 'init=/init_env_poweroff.sh - asdf=qwer zxcv' -n
