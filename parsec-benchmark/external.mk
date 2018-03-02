################################################################################
#
# PARSEC
#
################################################################################

PARSEC_VERSION = master
PARSEC_SITE = git@github.com:cirosantilli/parsec-benchmark.git
PARSEC_SITE_METHOD = git

define PARSEC_BUILD_CMDS
	cd $(@D) && . env.sh && for pkg in $(BR2_PACKAGE_PARSEC_BUILD_LIST); do parsecmgmt -a build -p $$pkg; done
	#cd '$(@D)/gem5/util/m5' && $(MAKE) -f 'Makefile.$(ARCH_MAKE)' CC='$(TARGET_CC)' LD='$(TARGET_LD)'
endef

define PARSEC_INSTALL_TARGET_CMDS
	# This is a bit coarse and makes the image huge with useless source code,
	# and input files, but I don't feel like creating per-package installs.
	# And it doesn't matter much for simulators anyways.
	rsync -av '$(@D)/' '$(TARGET_DIR)/parsec'
endef

$(eval $(generic-package))
