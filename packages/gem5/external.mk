################################################################################
#
# GEM5
#
################################################################################

GEM5_VERSION = 1.0
GEM5_SITE = $(GEM5_LKMC_SRCDIR)
GEM5_SITE_METHOD = local

ifeq ($(ARCH),x86_64)
ARCH_MAKE = x86
else
ARCH_MAKE = $(ARCH)
endif

define GEM5_BUILD_CMDS
	# TODO cannot use TARGET_CONFIGURE_OPTS here because it overrides the CFLAGS on m5,
	# which have an include. We should patch gem5 to add a += instead of = there.
	cd '$(@D)/util/m5' && $(MAKE) -f 'Makefile.$(ARCH_MAKE)' CC='$(TARGET_CC)' LD='$(TARGET_LD)'
endef

define GEM5_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 '$(@D)/util/m5/m5' '$(TARGET_DIR)/usr/bin'
endef

$(eval $(generic-package))
