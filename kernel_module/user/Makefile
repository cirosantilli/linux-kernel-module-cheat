.PHONY: all clean

CFLAGS_EXTRA ?= -fopenmp -std=c99 -Wall -Werror -Wextra
IN_EXT_C ?= .c
IN_EXT_CXX ?= .cpp
LIBS :=
OUT_EXT ?= .out

OUTS := $(foreach IN_EXT,$(IN_EXT_C) $(IN_EXT_CXX),$(addsuffix $(OUT_EXT), $(basename $(wildcard *$(IN_EXT)))))
ifeq ($(BR2_PACKAGE_EIGEN),y)
	CXXFLAGS_EXTRA += -I$(STAGING_DIR)/usr/include/eigen3
	# TODO: was failing with:
	# fatal error: Eigen/Dense: No such file or directory as of
	# 975ce0723ee3fa1fea1766e6683e2f3acb8558d6
	# http://lists.busybox.net/pipermail/buildroot/2018-June/222914.html
	#CXXFLAGS_EXTRA += $(shell $(PKG_CONFIG) --cflags eigen3)
else
	OUTS := $(filter-out eigen%$(OUT_EXT),$(OUTS))
endif
ifeq ($(BR2_PACKAGE_LIBDRM),y)
	LIBS += $(shell $(PKG_CONFIG) --libs libdrm)
	CFLAGS_EXTRA += $(shell $(PKG_CONFIG) --cflags libdrm)
else
	OUTS := $(filter-out libdrm_%$(OUT_EXT),$(OUTS))
endif
ifeq ($(BR2_PACKAGE_OPENBLAS),y)
	LIBS += -lopenblas
else
	OUTS := $(filter-out openblas$(OUT_EXT),$(OUTS))
endif

all: $(OUTS)

%$(OUT_EXT): %$(IN_EXT_C)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -o '$@' '$<' $(LIBS)

%$(OUT_EXT): %$(IN_EXT_CXX)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_EXTRA) -o '$@' '$<' $(LIBS)

clean:
	rm -f *'$(OUT_EXT)'
