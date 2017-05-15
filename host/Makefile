obj-m += hello.o
ccflags-y := -Wno-declaration-after-statement -std=gnu99

.PHONY: all clean

all: hello.ko

hello.ko: hello.c
	make -C '/lib/modules/$(shell uname -r)/build' M='$(PWD)' modules

clean:
	make -C '/lib/modules/$(shell uname -r)/build' M='$(PWD)' clean
