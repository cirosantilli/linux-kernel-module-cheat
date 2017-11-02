# Other architectures

The portability of the kernel and toolchains is amazing: change an option and most things magically work on completely different hardware.

## arm

First build:

    ./build -a arm
    ./run -a arm

Debug:

    ./run -a arm -d
    # On another terminal.
    ./rungdb -a arm

TODOs:

-   only managed to run in the terminal interface (but weirdly a blank QEMU window is still opened)
-   GDB not connecting to KGDB. Possibly linked to `-serial stdio`. See also: <https://stackoverflow.com/questions/14155577/how-to-use-kgdb-on-arm>

## aarch64

    ./build -a aarch64

TODOs:

-   GDB gives a ton of messages:

        no module object found for ''

    when you connect. `Ctrl + C` then `c` works though.

-   How to add devices to `-M virt` as we did for `-M versatilepb`

## mips64

    ./build -a mips64
