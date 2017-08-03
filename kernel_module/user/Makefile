.PHONY: all clean

CCC ?= gcc -ggdb3 -O0 -std=c99 -Wall -Werror -Wextra
IN_EXT ?= .c
OUT_EXT ?= .out

OUTS := $(addsuffix $(OUT_EXT), $(basename $(wildcard *$(IN_EXT))))

all: $(OUTS)

%$(OUT_EXT): %$(IN_EXT)
	$(CCC) -o '$@' '$<'

clean:
	rm -f *'$(OUT_EXT)'
