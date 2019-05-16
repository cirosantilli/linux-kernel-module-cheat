# Trying to do:
# $(MAKE) -C '$(LINUX_DIR)' M='$(M)' hello.ko hello2.ko
# to restrict which modules are built leads to failures
# when doing parallel builds. The only solution I could find
# was to let the host select obj-m itself.
obj-m += $(OBJECT_FILES)
ccflags-y := \
  -DDEBUG \
  -ggdb3 \
  -std=gnu99 \
  -Werror \
  -Wframe-larger-than=1000000000 \
  -Wno-declaration-after-statement \
  $(CCFLAGS)

.PHONY: all

all:
	$(MAKE) -C '$(LINUX_DIR)' M='$(M)'
