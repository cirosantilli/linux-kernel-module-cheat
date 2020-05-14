.POSIX:

CC = gcc
CXX = g++
CFLGS = -std=c99
CCFLGS = -ggdb3 -O0 -pedantic-errors -Wall -Wextra -Wno-missing-field-initializers -fpie
CXXFLGS = -std=c++11
IN_EXT = .c
IN_CXX_EXT = .cpp
OUT_EXT = .out

OUTS = $(addsuffix $(OUT_EXT), $(basename $(wildcard *$(IN_EXT))) $(basename $(wildcard *$(IN_CXX_EXT))))

.PHONY: all clean test

all: $(OUTS)

%$(OUT_EXT): %$(IN_EXT)
	$(CC) $(CFLGS) $(CCFLGS) $$(python3-config --cflags --embed) -o '$@' '$<' $$(python3-config --embed --ldflags)

%$(OUT_EXT): %$(IN_CXX_EXT)
	$(CXX) $(CXXFLGS) $(CCFLGS) $$(python3-config --cflags --embed) -o '$@' '$<' $$(python3-config --embed --ldflags)

clean:
	rm -rf *'$(OUT_EXT)' __pycache__ *.pyc

test: all
	@\
	for t in *"$(OUT_EXT)"; do\
	  if ! ./"$$t"; then\
		echo "ASSERT FAILED: $$t";\
		exit 1;\
	  fi;\
	done;\
