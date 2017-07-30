.PHONY: clean

CC ?= gcc -ggdb3 -O0 -std=c99 -Wall -Werror -Wextra
IN_EXT ?= .c
OUT_EXT ?= .out

OUTS := $(addsuffix $(OUT_EXT), $(basename $(wildcard *$(IN_EXT))))

all: $(OUTS)

%$(OUT_EXT): %$(IN_EXT)
	$(CC) -o '$@' '$<'

clean:
	rm -f *'$(OUT_EXT)'
