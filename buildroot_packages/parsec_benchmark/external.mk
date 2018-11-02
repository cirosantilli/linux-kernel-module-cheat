################################################################################
#
# PARSEC_BENCHMARK
#
################################################################################

# If we ever upstream to Buildroot.
# PARSEC_BENCHMARK_VERSION = 75d55ac446a43c47efb1044844a108c6c330184c
# PARSEC_BENCHMARK_SITE = git@github.com:cirosantilli/parsec-benchmark.git
# PARSEC_BENCHMARK_SITE_METHOD = git

PARSEC_BENCHMARK_VERSION = 1.0
PARSEC_BENCHMARK_SITE = $(LKMC_PARSEC_BENCHMARK_SRCDIR)
PARSEC_BENCHMARK_SITE_METHOD = local

define PARSEC_BENCHMARK_BUILD_CMDS
  # TODO make this nicer, only untar when extract step is done.
  # EXTRACT_CMDS and EXTRA_DOWNLOADS would be good candidates,
  # but they don't run with OVERRIDE_SRCDIR.
  '$(@D)/get-inputs' $(if $(filter $(V),1),-v,) '$(DL_DIR)' '$(@D)/'
  # We run the benchmarks with the wrong arch here to generate the inputs on the host.
  # This is because on gem5 this takes too long to do.
  export GNU_TARGET_NAME='$(GNU_TARGET_NAME)' && \
  export GNU_HOST_NAME='$(GNU_HOST_NAME)' && \
  export HOSTCC='$(HOSTCC)' && \
  export M4='$(HOST_DIR)/usr/bin/m4' && \
  export MAKE='$(MAKE)' && \
  export OSTYPE=linux && \
  export TARGET_CROSS='$(TARGET_CROSS)' && \
  export CC="$(TARGET_CC)" && \
  export CPP="$(TARGET_CPP)" && \
  export CXX="$(TARGET_CXX)" && \
  export HOSTTYPE=$(BR2_ARCH) && \
  cd $(@D) && \
  . env.sh && \
  for pkg in $(BR2_PACKAGE_PARSEC_BENCHMARK_BUILD_LIST); do \
    parsecmgmt -a build -p $$pkg && \
    if [ ! '$(BR2_PACKAGE_PARSEC_BENCHMARK_PARSECMGMT)' = y ]; then \
      parsecmgmt -a run -p $$pkg -i $(BR2_PACKAGE_PARSEC_BENCHMARK_INPUT_SIZE); \
    fi \
  done
endef

define PARSEC_BENCHMARK_INSTALL_TARGET_CMDS
  mkdir -p '$(TARGET_DIR)/parsec/'
  $(INSTALL) -D -m 0755 '$(@D)/test.sh' '$(TARGET_DIR)/parsec/'
  if [ '$(BR2_PACKAGE_PARSEC_BENCHMARK_PARSECMGMT)' = y ]; then \
    rsync -am $(if $(filter $(V),1),-v,) \
      --exclude '**/obj/' \
      --exclude '**/run/' \
      --exclude '**/src/' \
      '$(@D)/' '$(TARGET_DIR)/parsec/' \
    ; \
  else \
    rsync -am $(if $(filter $(V),1),-v,) --include '*/' \
      --include 'inst/***' \
      --include 'run/***' \
      --exclude '*' '$(@D)/' '$(TARGET_DIR)/parsec/' \
    ; \
  fi
  # rsync finished.
endef

$(eval $(generic-package))
