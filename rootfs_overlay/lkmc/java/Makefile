IN_EXT ?= .java
OUT_EXT ?= .class
RUN ?= Main
TEST ?= test

OUTS := $(addsuffix $(OUT_EXT), $(basename $(wildcard *$(IN_EXT))))

-include Makefile_params

.PHONY: all clean run

all:
	javac *.java

clean:
	rm -f *$(OUT_EXT)

run: all
	java -ea $(RUN)

test: all
	@\
	if [ -x $(TEST) ]; then \
	  ./$(TEST) '$(OUTS)' ;\
	else\
	  fail=false ;\
	  for t in $(basename $(OUTS)); do\
	    if ! java -ea "$$t"; then \
	      fail=true ;\
	      break ;\
	    fi ;\
	  done ;\
	  if $$fail; then \
	    echo "TEST FAILED: $$t" ;\
	    exit 1 ;\
	  else \
	    echo 'ALL TESTS PASSED' ;\
	    exit 0 ;\
	  fi ;\
	fi ;\
