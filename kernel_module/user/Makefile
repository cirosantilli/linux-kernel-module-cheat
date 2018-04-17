.PHONY: all clean

CFLAGS_EXTRA ?= -fopenmp -std=c99 -Wall -Werror -Wextra
IN_EXT ?= .c
LIBS :=
OUT_EXT ?= .out

OUTS := $(addsuffix $(OUT_EXT), $(basename $(wildcard *$(IN_EXT))))
ifeq ($(BR2_PACKAGE_OPENBLAS),y)
	LIBS += -lopenblas
else
	OUTS := $(filter-out openblas.out,$(OUTS))
endif

all: $(OUTS)

%$(OUT_EXT): %$(IN_EXT)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -o '$@' '$<' $(LIBS)

clean:
	rm -f *'$(OUT_EXT)'
