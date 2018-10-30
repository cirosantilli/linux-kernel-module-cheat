.PHONY: all clean mkdir

CFLAGS = -fopenmp -std=c99 $(CCFLAGS) $(CFLAGS_EXTRA)
CXXFLAGS = -std=c++17 $(CCFLAGS) $(CXXFLAGS_EXTRA)
CCFLAGS = -ggdb3 -O0 -Wall -Werror -Wextra -Wno-unused-function $(CCFLAGS_EXTRA)
IN_EXT_C = .c
IN_EXT_CXX = .cpp
LIBS = -lm
OUT_EXT = .out
OUT_DIR = .

OUTS := $(foreach IN_EXT,$(IN_EXT_C) $(IN_EXT_CXX),$(addsuffix $(OUT_EXT), $(basename $(wildcard *$(IN_EXT)))))
ifeq ($(HAS_EIGEN),y)
	CXXFLAGS_EXTRA += -I$(STAGING_DIR)/usr/include/eigen3
	# TODO: was failing with:
	# fatal error: Eigen/Dense: No such file or directory as of
	# 975ce0723ee3fa1fea1766e6683e2f3acb8558d6
	# http://lists.busybox.net/pipermail/buildroot/2018-June/222914.html
	#CXXFLAGS_EXTRA += $(shell $(PKG_CONFIG) --cflags eigen3)
else
	OUTS := $(filter-out eigen_%$(OUT_EXT),$(OUTS))
endif
ifeq ($(HAS_LIBDRM),y)
	LIBS += $(shell $(PKG_CONFIG) --libs libdrm)
	CFLAGS_EXTRA += $(shell $(PKG_CONFIG) --cflags libdrm)
else
	OUTS := $(filter-out libdrm_%$(OUT_EXT),$(OUTS))
endif
ifeq ($(HAS_OPENBLAS),y)
	LIBS += $(shell $(PKG_CONFIG) --libs openblas)
	CFLAGS_EXTRA += $(shell $(PKG_CONFIG) --cflags openblas)
else
	OUTS := $(filter-out openblas_%$(OUT_EXT),$(OUTS))
endif
OUTS := $(addprefix $(OUT_DIR)/,$(OUTS))

all: mkdir $(OUTS)

$(OUT_DIR)/%$(OUT_EXT): %$(IN_EXT_C)
	$(CC) $(CFLAGS) -o '$@' '$<' $(LIBS)

$(OUT_DIR)/%$(OUT_EXT): %$(IN_EXT_CXX)
	$(CXX) $(CXXFLAGS) -o '$@' '$<' $(LIBS)

clean:
	rm -f *'$(OUT_EXT)'

mkdir:
	mkdir -p '$(OUT_DIR)'
