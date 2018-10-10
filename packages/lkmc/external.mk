################################################################################
#
# kernel_modules
#
################################################################################

LKMC_VERSION = 1.0
LKMC_SITE = $(BR2_EXTERNAL_LKMC_PATH)
LKMC_SITE_METHOD = local
LKMC_MODULE_SUBDIRS = kernel_modules

ifeq ($(BR2_PACKAGE_EIGEN),y)
	LKMC_DEPENDENCIES += eigen
endif
ifeq ($(BR2_PACKAGE_LIBDRM),y)
	LKMC_DEPENDENCIES += libdrm
endif
ifeq ($(BR2_PACKAGE_OPENBLAS),y)
	LKMC_DEPENDENCIES += openblas
endif

define LKMC_BUILD_CMDS
	$(MAKE) -C '$(@D)/userland' $(TARGET_CONFIGURE_OPTS) \
	  HAS_EIGEN="$(BR2_PACKAGE_EIGEN)" \
	  HAS_LIBDRM="$(BR2_PACKAGE_LIBDRM)" \
	  HAS_OPENBLAS="$(BR2_PACKAGE_OPENBLAS)" \
	;
endef

define LKMC_INSTALL_TARGET_CMDS
	# The modules are already installed by the kernel-module package type
	# under /lib/modules/**, but let's also copy the modules to the root
	# for insmod convenience.
	#
	# Modules can be still be easily inserted with "modprobe module" however.
	$(INSTALL) -D -m 0755 $(@D)/userland/*.out '$(TARGET_DIR)'
endef

$(eval $(kernel-module))
$(eval $(generic-package))
