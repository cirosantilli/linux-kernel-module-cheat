################################################################################
#
# PARSEC_BENCHMARK
#
################################################################################

PARSEC_BENCHMARK_VERSION = master
PARSEC_BENCHMARK_SITE = git@github.com:cirosantilli/parsec-benchmark.git
PARSEC_BENCHMARK_SITE_METHOD = git

define PARSEC_BENCHMARK_BUILD_CMDS
  # TODO make this nicer, only untar when extract step is done.
  # EXTRACT_CMDS and EXTRA_DOWNLOADS would be good candidates,
  # but they don't run with OVERRIDE_SRCDIR.
  '$(PARSEC_BENCHMARK_PKGDIR)/parsec-benchmark/get-inputs' $(if $(filter $(V),1),-v,) '$(DL_DIR)' '$(@D)/'
  # We run the benchmarks with the wrong arch here to generate the inputs on the host.
  # This is because on gem5 this takes too long to do.
  cd $(@D) && . env.sh && for pkg in $(BR2_PACKAGE_PARSEC_BENCHMARK_BUILD_LIST); do \
    export HOSTCC='$(HOSTCC)'; \
    export M4='$(HOST_DIR)/usr/bin/m4'; \
    export MAKE='$(MAKE)'; \
    export OSTYPE=linux; \
    export TARGET_CROSS='$(TARGET_CROSS)'; \
    export HOSTTYPE='$(BR2_ARCH)'; \
    parsecmgmt -a build -p $$pkg; \
    parsecmgmt -a run -p $$pkg -i $(BR2_PACKAGE_PARSEC_BENCHMARK_INPUT_SIZE); \
  done
endef

define PARSEC_BENCHMARK_INSTALL_TARGET_CMDS
  # This is a bit coarse and makes the image larger with useless source code.
  #
  # But according to du, the source accounts for only 1/5 of the total size,
  # so benchmarks dominate, and it doesn't matter much.
  #
  # Also it is not so critical for simulators anyways unlike real embedded systems.
  #
  # One possibility to make this better may be to install only the 'inst/' and 'input/'
  # folders for each package + toplevel '/bin/' and '/config/', but of course we won't
  # know if this works until time consuming testing is done :-)
  mkdir -p '$(TARGET_DIR)/parsec/'

  rsync -am $(if $(filter $(V),1),-v,) --include '*/' \
    --include '/bin/***' \
    --include '/config/***' \
    --include '/env.sh' \
    --include 'inst/***' \
    --include 'run/***' \
    --exclude '*' '$(@D)/' '$(TARGET_DIR)/parsec/' \
  ;
  # rsync finished.
endef

$(eval $(generic-package))
