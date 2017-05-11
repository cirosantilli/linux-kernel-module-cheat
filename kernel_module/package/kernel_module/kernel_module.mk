################################################################################
#
# kernel_module
#
################################################################################

KERNEL_MODULE_VERSION = 1.0
KERNEL_MODULE_SITE = $(BR2_EXTERNAL_KERNEL_MODULE_PATH)/package/kernel_module/src
KERNEL_MODULE_SITE_METHOD = local

define KERNEL_MODULE_BUILD_CMDS
	$(MAKE) -C '$(@D)' LINUX_DIR='$(LINUX_DIR)' PWD='$(@D)' CC='$(TARGET_CC)' LD='$(TARGET_LD)'
endef

define KERNEL_MODULE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/*.ko '$(TARGET_DIR)'
endef

$(eval $(kernel-module))
$(eval $(generic-package))
