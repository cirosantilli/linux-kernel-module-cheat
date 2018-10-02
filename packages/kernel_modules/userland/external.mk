################################################################################
#
# kernel_modules
#
################################################################################

KERNEL_MODULES_VERSION = 1.0
KERNEL_MODULES_SITE = $(BR2_EXTERNAL_KERNEL_MODULES_PATH)
KERNEL_MODULES_SITE_METHOD = local

ifeq ($(BR2_PACKAGE_EIGEN),y)
	KERNEL_MODULES_DEPENDENCIES += eigen
endif
ifeq ($(BR2_PACKAGE_LIBDRM),y)
	KERNEL_MODULES_DEPENDENCIES += libdrm
endif
ifeq ($(BR2_PACKAGE_OPENBLAS),y)
	KERNEL_MODULES_DEPENDENCIES += openblas
endif

define KERNEL_MODULES_BUILD_CMDS
	$(MAKE) -C '$(@D)' $(TARGET_CONFIGURE_OPTS) \
	  BR2_PACKAGE_EIGEN="$(BR2_PACKAGE_EIGEN)" \
	  BR2_PACKAGE_LIBDRM="$(BR2_PACKAGE_LIBDRM)" \
	  BR2_PACKAGE_OPENBLAS="$(BR2_PACKAGE_OPENBLAS)" \
	;
endef

define KERNEL_MODULES_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/*.out '$(TARGET_DIR)'
endef

$(eval $(kernel-module))
$(eval $(generic-package))
