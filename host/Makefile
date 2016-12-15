obj-m += hello.o
ccflags-y := -Wno-declaration-after-statement -std=gnu99

.PHONY: all clean

all: hello.ko ins_rm_mod.out

hello.ko: hello.c
	make -C '/lib/modules/$(shell uname -r)/build' M='$(PWD)' modules

clean:
	make -C '/lib/modules/$(shell uname -r)/build' M='$(PWD)' clean
	rm -f ins_rm_mod.out

ins_rm_mod.out: ins_rm_mod.c hello.c
	gcc -Wall -std=gnu99 -o ins_rm_mod.out ins_rm_mod.c
