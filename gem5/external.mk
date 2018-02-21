################################################################################
#
# GEM5
#
################################################################################

GEM5_VERSION = 1.0
GEM5_SITE = $(BR2_EXTERNAL_GEM5_PATH)
GEM5_SITE_METHOD = local

define GEM5_BUILD_CMDS
	cd '$(@D)/gem5/util/m5' && $(MAKE) -f 'Makefile.$(ARCH)' CC='$(TARGET_CC)' LD='$(TARGET_LD)'
endef

define GEM5_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 '$(@D)/gem5/util/m5/m5' '$(TARGET_DIR)'
endef

$(eval $(generic-package))
