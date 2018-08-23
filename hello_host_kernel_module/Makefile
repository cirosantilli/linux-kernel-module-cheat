obj-m += hello.o
ccflags-y := -Wno-declaration-after-statement -std=gnu99

.PHONY: all clean

all: hello.ko

hello.ko: hello.c
	$(MAKE) -C '/lib/modules/$(shell uname -r)/build' M='$(PWD)' modules

clean:
	$(MAKE) -C '/lib/modules/$(shell uname -r)/build' M='$(PWD)' clean
