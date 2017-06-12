.PHONY: clean

CC ?= gcc
IN_EXT ?= .c
OUT_EXT ?= .out

OUTS := $(addsuffix $(OUT_EXT), $(basename $(wildcard *$(IN_EXT))))

all: $(OUTS)

%$(OUT_EXT): %$(IN_EXT)
	$(CC) -o '$@' '$<'

clean:
	rm -f *'$(OUT_EXT)'
