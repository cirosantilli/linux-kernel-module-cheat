################################################################################
#
# kernel_module
#
################################################################################

KERNEL_MODULE_VERSION = 1.0
KERNEL_MODULE_SITE = $(BR2_EXTERNAL_KERNEL_MODULE_PATH)
KERNEL_MODULE_SITE_METHOD = local

ifeq ($(BR2_PACKAGE_OPENBLAS),y)
	KERNEL_MODULE_DEPENDENCIES += openblas
endif
ifeq ($(BR2_PACKAGE_EIGEN),y)
	KERNEL_MODULE_DEPENDENCIES += eigen
endif

define KERNEL_MODULE_BUILD_CMDS
	$(MAKE) -C '$(@D)/user' $(TARGET_CONFIGURE_OPTS) \
	  BR2_PACKAGE_OPENBLAS="$(BR2_PACKAGE_OPENBLAS)" \
	  BR2_PACKAGE_EIGEN="$(BR2_PACKAGE_EIGEN)"
endef

define KERNEL_MODULE_INSTALL_TARGET_CMDS
	# The modules are already installed by the kernel-module package type
	# under /lib/modules/**, but let's also copy the modules to the root
	# for insmod convenience.
	#
	# Modules can be still be easily inserted with "modprobe module" however.
	$(INSTALL) -D -m 0655 $(@D)/*.ko '$(TARGET_DIR)'
	$(INSTALL) -D -m 0755 $(@D)/user/*.out '$(TARGET_DIR)'
endef

$(eval $(kernel-module))
$(eval $(generic-package))
