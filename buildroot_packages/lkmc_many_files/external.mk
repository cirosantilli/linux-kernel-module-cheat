################################################################################
#
# lkmc_many_files
#
################################################################################

LKMC_MANY_FILES_VERSION = 1.0
LKMC_MANY_FILES_SITE = $(BR2_EXTERNAL_LKMC_MANY_FILES_PATH)
LKMC_MANY_FILES_SITE_METHOD = local

define LKMC_MANY_FILES_BUILD_CMDS
	# D contains the source code of this package.
	# TARGET_CONFIGURE_OPTS contains several common options such as CFLAGS and LDFLAGS.
	$(MAKE) -C '$(@D)' $(TARGET_CONFIGURE_OPTS)
endef

define LKMC_MANY_FILES_INSTALL_TARGET_CMDS
  # Anything put inside TARGET_DIR will end up on the guest relative to the root directory.
  i=0; \
  while [ $$i -le 128 ]; do \
    j=0; \
    while [ $$j -le 128 ]; do \
      $(INSTALL) -D -m 0755 $(@D)/lkmc_many_files.out $(TARGET_DIR)/lkmc_many_files/$${i}_$${j}.out; \
      j=$$(($$j+1)); \
    done; \
    i=$$(($$i+1)); \
  done
endef

$(eval $(generic-package))
