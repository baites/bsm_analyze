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
lib = ./lib/libbsm_test.so.1.1

# Get list of all heads, sources and objects. Each source (%.cc) whould have
# an object file
#
heads = $(wildcard ./interface/*.h)
srcs = $(wildcard ./src/*.cc)

objs = $(foreach obj,${srcs},$(addprefix ./obj/,$(patsubst %.cc,%.o,$(notdir ${obj}))))

# List of programs with main functions to be filtered out of objects
#
progs = $(foreach prog,$(wildcard ./src/*.cpp),$(addprefix ./bin/bsm_test_,$(patsubst ./src/%.cpp,%,${prog})))

# Need to operate with copies of the env variables b/c the latter ones are
# passed to sumbodules and shoould remain the same
#
cppflags = ${CPPFLAGS} ${debug} -fPIC -pipe -Wall -I../ -I$(shell root-config --incdir) -DSTANDALONE -I../bsm_input/message -I./
ldflags = ${LDFLAGS} $(shell root-config --libs) -L../lib -L./lib $(foreach mod,${submod},$(addprefix -l,${mod})) -lboost_filesystem -lboost_system -lboost_program_options -lboost_regex -lprotobuf
ifeq ($(shell uname),Linux)
	ldflags  += -L/usr/lib64 -lboost_thread
else
	cppflags += -I/opt/local/include
	ldflags  += -L/opt/local/lib -lboost_thread-mt
endif

# Rules to be always executed: empty ones
.PHONY: prog

lib: ${lib}

all: prog

obj: ${objs}

prog: ${progs}



# Regular compilcation
#
${objs}: obj/%.o: src/%.cc interface/%.h
	@echo "[+] Compiling objects $@ ..."
	${CXX} ${cppflags} -c $(addprefix ./src/,$(patsubst %.o,%.cc,$(notdir $@))) -o $@
	@echo



# Libraries
#
${lib}: ${objs}
	@echo "[+] Generating Library $@ ..."
	$(eval lib_name=$(notdir $@))
	${CXX} -shared -W1,-soname,${lib_name} ${ldflags} -o $@ ${objs}
	@cd ./lib; ln -fs ${lib_name} $(basename ${lib_name}); ln -fs $(basename ${lib_name}) $(basename $(basename ${lib_name}))
	@echo



# Executables
#
${progs}: bin/bsm_test_%: src/%.cpp ${lib}
	@echo "[+] Compiling programs $@ ..."
	$(eval prog_name=$(patsubst bin/bsm_test_%,%,$@))
	${CXX} ${cppflags} -c src/${prog_name}.cpp -o ./obj/${prog_name}.o
	${CXX} ${ldflags} ${lib} ./obj/${prog_name}.o -o $@
	@echo

# Cleaning
#
cleanbin:
ifneq ($(strip ${progs}),)
	rm -f ./bin/bsm_test_*
endif

clean: cleanbin
	rm -f ./obj/*.o
	rm -f ./lib/*
