# GEM5

TODO get working with a Buildroot recent Linux kernel. GEM5 is used mostly by chip makers, and they they keep everything that really works closed... This documents my failed attempts.

Related threads:

- <https://www.mail-archive.com/gem5-users@gem5.org/msg11384.html>
- <https://stackoverflow.com/questions/37906425/booting-gem5-x86-ubuntu-full-system-simulation>

Vs QEMU:

-   advantage: simulates a generic more realistic CPU cycle by cycle, e.g. memory access, caches and page table manipulations. This allows us to:
    - do much more realistic performance benchmarking with it
    - make functional cache observations, e.g. to use Linux kernel APIs that flush memory like DMA, and are used widely in drivers
-   disadvantage: slow

## Working baseline

Working x86 with the pre-built magic image with an ancient 2.6.22.9 kernel starting point:

    sudo mkdir -p /dist/m5/system
    sudo chmod 777 /dist/m5/system
    cd /dist/m5/system
    # Backed up at:
    # https://github.com/cirosantilli/media/releases/tag/gem5
    wget http://www.gem5.org/dist/current/x86/x86-system.tar.bz2
    tar xvf x86-system.tar.bz2
    cd x86-system
    dd if=/dev/zero of=disks/linux-bigswap2.img bs=1024 count=65536
    mkswap disks/linux-bigswap2.img
    cd ..

    sudo apt-get install libgoogle-perftools-dev mercurial protobuf-compiler
    git clone https://gem5.googlesource.com/public/gem5
    cd gem5
    git checkout e519aa4ac2942915355f0ef12e88286322336419
    scons -j$(nproc) build_opts/X86/gem5.opt
    # That old blob has wrong filenames.
    ./build/X86/gem5.opt \
        -d /tmp/output \
        --disk-image=/dist/m5/system/disks/linux-x86.img \
        --kernel=/dist/m5/system/binaries/x86_64-vmlinux-2.6.22.9 \
        configs/example/fs.py

On another shell:

    telnet localhost 3456

## Unmodified Buildroot images

bzImage fails, so we always try with vmlinux obtained from inside build/.

rootfs.ext2 and vmlinux from 670366caaded57d318b6dbef34e863e3b30f7f29ails as:

Fails as:

    Global frequency set at 1000000000000 ticks per second
    warn: DRAM device capacity (8192 Mbytes) does not match the address range assigned (512 Mbytes)
    info: kernel located at: /data/git/linux-kernel-module-cheat/buildroot/output.x86_64~/build/linux-custom/vmlinux
    Listening for com_1 connection on port 3456
        0: rtc: Real-time clock set to Sun Jan  1 00:00:00 2012
    0: system.remote_gdb.listener: listening for remote gdb #0 on port 7000
    warn: Reading current count from inactive timer.
    **** REAL SIMULATION ****
    info: Entering event queue @ 0.  Starting simulation...
    warn: instruction 'fninit' unimplemented
    warn: Don't know what interrupt to clear for console.
    12516923000: system.pc.com_1.terminal: attach terminal 0
    warn: i8042 "Write output port" command not implemented.
    warn: i8042 "Write keyboard output buffer" command not implemented.
    warn: Write to unknown i8042 (keyboard controller) command port.
    hack: Assuming logical destinations are 1 << id.
    panic: Resetting mouse wrap mode unimplemented.
    Memory Usage: 1003456 KBytes
    Program aborted at tick 632745027500
    --- BEGIN LIBC BACKTRACE ---
    ./build/X86/gem5.opt(_Z15print_backtracev+0x15)[0x12b8165]
    ./build/X86/gem5.opt(_Z12abortHandleri+0x39)[0x12c32f9]
    /lib/x86_64-linux-gnu/libpthread.so.0(+0x11390)[0x7fe047a71390]
    /lib/x86_64-linux-gnu/libc.so.6(gsignal+0x38)[0x7fe046601428]
    /lib/x86_64-linux-gnu/libc.so.6(abort+0x16a)[0x7fe04660302a]
    ./build/X86/gem5.opt(_ZN6X86ISA8PS2Mouse11processDataEh+0xf5)[0x1391095]
    ./build/X86/gem5.opt(_ZN6X86ISA5I80425writeEP6Packet+0x51c)[0x13927ec]
    ./build/X86/gem5.opt(_ZN7PioPort10recvAtomicEP6Packet+0x66)[0x139f7b6]
    ./build/X86/gem5.opt(_ZN15NoncoherentXBar10recvAtomicEP6Packets+0x200)[0x1434af0]
    ./build/X86/gem5.opt(_ZN6Bridge15BridgeSlavePort10recvAtomicEP6Packet+0x5d)[0x140ee9d]
    ./build/X86/gem5.opt(_ZN12CoherentXBar10recvAtomicEP6Packets+0x3e7)[0x1415b77]
    ./build/X86/gem5.opt(_ZN15AtomicSimpleCPU8writeMemEPhjm5FlagsIjEPm+0x327)[0xa790a7]
    ./build/X86/gem5.opt(_ZN17SimpleExecContext8writeMemEPhjm5FlagsIjEPm+0x19)[0xa856b9]
    ./build/X86/gem5.opt(_ZNK10X86ISAInst2St7executeEP11ExecContextPN5Trace10InstRecordE+0x235)[0xfb9e65]
    ./build/X86/gem5.opt(_ZN15AtomicSimpleCPU4tickEv+0x23c)[0xa784fc]
    ./build/X86/gem5.opt(_ZN10EventQueue10serviceOneEv+0xc5)[0x12be0d5]
    ./build/X86/gem5.opt(_Z9doSimLoopP10EventQueue+0x38)[0x12cd558]
    ./build/X86/gem5.opt(_Z8simulatem+0x2eb)[0x12cdbdb]
    ./build/X86/gem5.opt(_ZZN8pybind1112cpp_function10initializeIRPFP22GlobalSimLoopExitEventmES3_ImEINS_4nameENS_5scopeENS_7siblingENS_5arg_vEEEEvOT_PFT0_DpT1_EDpRKT2_ENUlRNS_6detail13function_callEE1_4_FUNESO_+0x41)[0x13fca11]
    ./build/X86/gem5.opt(_ZN8pybind1112cpp_function10dispatcherEP7_objectS2_S2_+0x8d8)[0xfc7398]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalFrameEx+0x7852)[0x7fe047d3b552]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalCodeEx+0x85c)[0x7fe047e6501c]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalFrameEx+0x6ffd)[0x7fe047d3acfd]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalFrameEx+0x7124)[0x7fe047d3ae24]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalFrameEx+0x7124)[0x7fe047d3ae24]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalCodeEx+0x85c)[0x7fe047e6501c]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalCode+0x19)[0x7fe047d33b89]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalFrameEx+0x613b)[0x7fe047d39e3b]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalCodeEx+0x85c)[0x7fe047e6501c]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalFrameEx+0x6ffd)[0x7fe047d3acfd]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalCodeEx+0x85c)[0x7fe047e6501c]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalCode+0x19)[0x7fe047d33b89]
    --- END LIBC BACKTRACE ---
    Aborted (core dumped)

Boot goes quite far, on telnet:

    ALSA device list:
      No soundcards found.

So just looks like we have to disable some Linux configs which GEM5 does not support... so fragile.

## Copy upstream 2.6 configs on 4.9 kernel

The magic image provides its kernel configurations, so let's try that.

The configs are present at:

    wget http://www.gem5.org/dist/current/x86/config-x86.tar.bz2

backed up at: <https://github.com/cirosantilli/media/releases/tag/gem5>

Copy `linux-2.6.22.9` into the kernel tree as `.config`, `git checkout v4.9.6`, `make olddefconfig`, `make`, then use the Buildroot filesystem as above, failure:

    panic: Invalid IDE control register offset: 0
    Memory Usage: 931272 KBytes
    Program aborted at tick 382834812000
    --- BEGIN LIBC BACKTRACE ---
    ./build/X86/gem5.opt(_Z15print_backtracev+0x15)[0x12b8165]
    ./build/X86/gem5.opt(_Z12abortHandleri+0x39)[0x12c32f9]
    /lib/x86_64-linux-gnu/libpthread.so.0(+0x11390)[0x7fc2081c6390]
    /lib/x86_64-linux-gnu/libc.so.6(gsignal+0x38)[0x7fc206d56428]
    /lib/x86_64-linux-gnu/libc.so.6(abort+0x16a)[0x7fc206d5802a]
    ./build/X86/gem5.opt(_ZN7IdeDisk11readControlEmiPh+0xd9)[0xa96989]
    ./build/X86/gem5.opt(_ZN13IdeController14dispatchAccessEP6Packetb+0x53e)[0xa947ae]
    ./build/X86/gem5.opt(_ZN13IdeController4readEP6Packet+0xe)[0xa94a5e]
    ./build/X86/gem5.opt(_ZN7PioPort10recvAtomicEP6Packet+0x3f)[0x139f78f]
    ./build/X86/gem5.opt(_ZN15NoncoherentXBar10recvAtomicEP6Packets+0x200)[0x1434af0]
    ./build/X86/gem5.opt(_ZN6Bridge15BridgeSlavePort10recvAtomicEP6Packet+0x5d)[0x140ee9d]
    ./build/X86/gem5.opt(_ZN12CoherentXBar10recvAtomicEP6Packets+0x3e7)[0x1415b77]
    ./build/X86/gem5.opt(_ZN15AtomicSimpleCPU7readMemEmPhj5FlagsIjE+0x3ef)[0xa780ef]
    ./build/X86/gem5.opt(_ZN17SimpleExecContext7readMemEmPhj5FlagsIjE+0x11)[0xa85671]
    ./build/X86/gem5.opt(_ZNK10X86ISAInst2Ld7executeEP11ExecContextPN5Trace10InstRecordE+0x130)[0xfb6c00]
    ./build/X86/gem5.opt(_ZN15AtomicSimpleCPU4tickEv+0x23c)[0xa784fc]
    ./build/X86/gem5.opt(_ZN10EventQueue10serviceOneEv+0xc5)[0x12be0d5]
    ./build/X86/gem5.opt(_Z9doSimLoopP10EventQueue+0x38)[0x12cd558]
    ./build/X86/gem5.opt(_Z8simulatem+0x2eb)[0x12cdbdb]
    ./build/X86/gem5.opt(_ZZN8pybind1112cpp_function10initializeIRPFP22GlobalSimLoopExitEventmES3_ImEINS_4nameENS_5scopeENS_7siblingENS_5arg_vEEEEvOT_PFT0_DpT1_EDpRKT2_ENUlRNS_6detail13function_callEE1_4_FUNESO_+0x41)[0x13fca11]
    ./build/X86/gem5.opt(_ZN8pybind1112cpp_function10dispatcherEP7_objectS2_S2_+0x8d8)[0xfc7398]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalFrameEx+0x7852)[0x7fc208490552]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalCodeEx+0x85c)[0x7fc2085ba01c]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalFrameEx+0x6ffd)[0x7fc20848fcfd]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalFrameEx+0x7124)[0x7fc20848fe24]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalFrameEx+0x7124)[0x7fc20848fe24]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalCodeEx+0x85c)[0x7fc2085ba01c]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalCode+0x19)[0x7fc208488b89]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalFrameEx+0x613b)[0x7fc20848ee3b]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalCodeEx+0x85c)[0x7fc2085ba01c]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalFrameEx+0x6ffd)[0x7fc20848fcfd]
    /usr/lib/x86_64-linux-gnu/libpython2.7.so.1.0(PyEval_EvalCodeEx+0x85c)[0x7fc2085ba01c]
    --- END LIBC BACKTRACE ---
    Aborted (core dumped)

## Use upstream 2.6 configs and 2.6 kernel

If we checkout to the ancient kernel `v2.6.22.9`, it fails to compile with modern GNU make 4.1: <https://stackoverflow.com/questions/35002691/makefile-make-clean-why-getting-mixed-implicit-and-normal-rules-deprecated-s> lol
