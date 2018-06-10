################################################################################
#
# sample_package
#
################################################################################

SAMPLE_PACKAGE_VERSION = 1.0
SAMPLE_PACKAGE_SITE = $(BR2_EXTERNAL_SAMPLE_PACKAGE_PATH)
SAMPLE_PACKAGE_SITE_METHOD = local

define SAMPLE_PACKAGE_BUILD_CMDS
	# D contains the source code of this package.
	# TARGET_CONFIGURE_OPTS contains several common options such as CFLAGS and LDFLAGS.
	$(MAKE) -C '$(@D)' $(TARGET_CONFIGURE_OPTS)
endef

define SAMPLE_PACKAGE_INSTALL_TARGET_CMDS
  # Anything put inside TARGET_DIR will end up on the guest relative to the root directory.
  $(INSTALL) -D -m 0755 $(@D)/*.out '$(TARGET_DIR)'
endef

$(eval $(generic-package))
