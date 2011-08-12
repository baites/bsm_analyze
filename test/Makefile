# Set CPPFLAGS environment variable for debugging:
#
# 	export CPPFLAGS=-gdwarf-2
#
# Created by Samvel Khalatyan, Aug 11, 2011
# Copyright 2011, All rights reserved

CXX ?= g++

# Subsystems that have compilable libraries
#
submod = bsm_core bsm_input bsm_stat JetMETObjects bsm_analyze
lib_submod = $(patsubst %,lib/lib%.so,${submod})

# List of programs with main functions to be filtered out of objects
#
progs = $(foreach prog,$(wildcard ./src/*.cpp),$(addprefix ./bin/bsm_,$(patsubst ./src/%.cpp,%,${prog})))

# Need to operate with copies of the env variables b/c the latter ones are
# passed to sumbodules and shoould remain the same
#
cppflags = ${CPPFLAGS} ${debug} -fPIC -pipe -Wall -I../ -I$(shell root-config --incdir) -DSTANDALONE -I./bsm_input/message
ldflags = ${LDFLAGS} $(shell root-config --libs) -L../lib $(foreach mod,${submod},$(addprefix -l,${mod})) -lboost_filesystem -lboost_system -lboost_program_options -lboost_regex -lprotobuf
ifeq ($(shell uname),Linux)
	ldflags  += -L/usr/lib64 -lboost_thread
else
	cppflags += -I/opt/local/include
	ldflags  += -L/opt/local/lib -lboost_thread-mt
endif

# Rules to be always executed: empty ones
.PHONY: prog

all: prog

prog: ${progs}

# Executables
#
${progs}: bin/bsm_%: src/%.cpp ${lib}
	@echo "[+] Compiling programs $@ ..."
	$(eval prog_name=$(patsubst bin/bsm_%,%,$@))
	${CXX} ${cppflags} -c src/${prog_name}.cpp -o ./obj/${prog_name}.o
	${CXX} ${ldflags} ${lib} ./obj/${prog_name}.o -o $@
	@echo

# Cleaning
#
clean:
ifneq ($(strip ${progs}),)
	rm -f ./bin/bsm_*
endif
