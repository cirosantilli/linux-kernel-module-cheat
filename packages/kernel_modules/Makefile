obj-m += $(OBJECT_FILES)
ccflags-y := -DDEBUG -g -std=gnu99 -Werror -Wno-declaration-after-statement -Wframe-larger-than=1000000000

.PHONY: all

all:
	$(MAKE) -C '$(LINUX_DIR)' M='$(M)'
