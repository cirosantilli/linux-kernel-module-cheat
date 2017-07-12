obj-m += $(addsuffix .o, $(notdir $(basename $(wildcard $(BR2_EXTERNAL_KERNEL_MODULE_PATH)/*.c))))
ccflags-y := -DDEBUG -g -std=gnu99 -Wno-declaration-after-statement

#ifneq ($(BR2_ARCH), x86_64)
#	obj-m := $(filter-out page_table_dump.o, $(obj-m))
#endif
#
#ifneq ($(BR2_ARCH), arm)
#	obj-m := $(filter-out platform_device.o, $(obj-m))
#endif

.PHONY: all clean

all:
	$(MAKE) -C '$(LINUX_DIR)' M='$(PWD)' modules

clean:
	$(MAKE) -C '$(LINUX_DIR)' M='$(PWD)' clean
