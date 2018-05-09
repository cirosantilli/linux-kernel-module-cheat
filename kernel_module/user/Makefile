.PHONY: all clean

CFLAGS_EXTRA ?= -fopenmp -std=c99 -Wall -Werror -Wextra
IN_EXT_C ?= .c
IN_EXT_CXX ?= .cpp
LIBS :=
OUT_EXT ?= .out

OUTS := $(foreach IN_EXT,$(IN_EXT_C) $(IN_EXT_CXX),$(addsuffix $(OUT_EXT), $(basename $(wildcard *$(IN_EXT)))))
ifeq ($(BR2_PACKAGE_OPENBLAS),y)
	LIBS += -lopenblas
else
	OUTS := $(filter-out openblas$(OUT_EXT),$(OUTS))
endif
ifeq ($(BR2_PACKAGE_EIGEN),y)
	# Header only.
	#LIBS += -leigen
	CXXFLAGS_EXTRA += $(shell $(PKG_CONFIG) --cflags --libs eigen3)
else
	OUTS := $(filter-out eigen%$(OUT_EXT),$(OUTS))
endif

all: $(OUTS)

%$(OUT_EXT): %$(IN_EXT_C)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -o '$@' '$<' $(LIBS)

%$(OUT_EXT): %$(IN_EXT_CXX)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_EXTRA) -o '$@' '$<' $(LIBS)

clean:
	rm -f *'$(OUT_EXT)'
