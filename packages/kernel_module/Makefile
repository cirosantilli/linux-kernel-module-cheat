obj-m += $(addsuffix .o, $(notdir $(basename $(filter-out %.mod.c, $(wildcard $(BR2_EXTERNAL_KERNEL_MODULE_PATH)/*.c)))))
ccflags-y := -DDEBUG -g -std=gnu99 -Werror -Wno-declaration-after-statement -Wframe-larger-than=1000000000

.PHONY: all clean

all:
	$(MAKE) -C '/lib/modules/$(shell uname -r)/build' M='$(PWD)' modules

clean:
	$(MAKE) -C '$(LINUX_DIR)' M='$(PWD)' clean
