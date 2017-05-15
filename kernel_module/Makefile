obj-m += $(addsuffix .o, $(notdir $(basename $(wildcard $(BR2_EXTERNAL_KERNEL_MODULE_PATH)/*.c))))
ccflags-y := -Wno-declaration-after-statement -std=gnu99

.PHONY: all clean test

all: test
	$(MAKE) -C '$(LINUX_DIR)' M='$(PWD)' modules

clean:
	$(MAKE) -C '$(LINUX_DIR)' M='$(PWD)' clean

test:
	$(MAKE) -C '$(PWD)/test'
