ifeq ($(OBJECT_FILES),)
obj-m += $(addsuffix .o, $(notdir $(basename $(filter-out %.mod.c, $(wildcard $(BR2_EXTERNAL_KERNEL_MODULES_PATH)/*.c)))))
else
# Trying to do:
# $(MAKE) -C '$(LINUX_DIR)' M='$(M)' hello.ko hello2.ko
# to restrict which modules are built leads to failures
# when doing parallel builds. The only solution I could find
# was to let the host select obj-m itself.
obj-m += $(OBJECT_FILES)
endif
ccflags-y := -DDEBUG -g -std=gnu99 -Werror -Wno-declaration-after-statement -Wframe-larger-than=1000000000

.PHONY: all

all:
	$(MAKE) -C '$(LINUX_DIR)' M='$(M)'
