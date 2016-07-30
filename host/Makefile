OUT_EXT := .ko
OBJ_EXT := .o
RUN     := hello
RUN_EXT := $(RUN)$(OUT_EXT)

obj-m += hello.o
ccflags-y := -Wno-declaration-after-statement -std=gnu99

.PHONY: clean ins log rm run my-ins

all: $(RUN_EXT) ins_rm_mod.out

hello.ko: hello.c
	make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" clean
	rm -f *.out

ins: all
	sudo insmod '$(RUN_EXT)'

log:
	dmesg

rm:
	if lsmod | grep -Eq '^$(RUN) '; then sudo rmmod '$(RUN_EXT)'; fi

ins_rm_mod.out: ins_rm_mod.c
	gcc -Wall -std=gnu99 -o '$@' '$<'

ins_rm_run: ins_rm_mod.out $(RUN_EXT)
	sudo ./ins_rm_mod.out
