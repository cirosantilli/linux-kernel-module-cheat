################################################################################
#
# kernel_modules
#
################################################################################

KERNEL_MODULES_VERSION = 1.0
KERNEL_MODULES_SITE = $(BR2_EXTERNAL_KERNEL_MODULES_PATH)
KERNEL_MODULES_SITE_METHOD = local

$(eval $(kernel-module))
$(eval $(generic-package))
