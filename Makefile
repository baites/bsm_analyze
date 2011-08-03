# Set CPPFLAGS environment variable for debugging:
#
# 	export CPPFLAGS=-gdwarf-2
#
# Created by Samvel Khalatyan, Aug 03, 2011
# Copyright 2011, All rights reserved

CXX ?= g++

# Subsystems that have compilable libraries
#
submod   = bsm_core bsm_input bsm_stat JetMETObjects
lib		 = libbsm_analyze.so.1.2

# Get list of all heads, sources and objects. Each source (%.cc) whould have
# an object file
#
heads    = $(wildcard ./interface/*.h)
srcs     = $(wildcard ./src/*.cc)

objs       = $(foreach obj,$(addprefix ./obj/,$(patsubst %.cc,%.o,$(notdir ${srcs}))),${obj})

# List of programs with main functions to be filtered out of objects
#
progs    = $(patsubst ./src/%.cpp,%,$(wildcard ./src/*.cpp))

# Special rules to clean each subsystem
#
clean_mod = $(addprefix CLN_,${submod})

# Need to operate with copies of the env variables b/c the latter ones are
# passed to sumbodules and shoould remain the same
#
cppflags = ${CPPFLAGS} ${debug} -fPIC -pipe -Wall -I./ -I$(shell root-config --incdir) -DSTANDALONE -I./bsm_input/message
ldflags  = ${LDFLAGS} $(shell root-config --libs) -L./lib $(foreach mod,${submod},$(addprefix -l,${mod})) -lboost_filesystem -lboost_system -lboost_program_options -lboost_regex -lprotobuf
ifeq ($(shell uname),Linux)
	ldflags  += -L/usr/lib64 -lboost_thread
else
	cppflags += -I/opt/local/include
	ldflags  += -L/opt/local/lib -lboost_thread-mt
endif

# Rules to be always executed: empty ones
#
.PHONY: ${submod} lib

lib: ${lib}

all: submod obj lib prog

help:
	@echo "make <rule>"
	@echo
	@echo "Rules"
	@echo "-----"
	@echo
	@echo "  obj        compile objects"
	@echo "  lib        compile shared library"
	@echo


submod: ${submod}

obj: submod ${objs}

prog: submod obj lib ${progs}

cleanall: clean ${clean_mod}



# Compile modules
#
${submod}: $(addprefix ./lib/lib,$(addsuffix .so,$@))
	@echo "[+] Compiling sub-module ..."
	${MAKE} -C $@
	@for lib in `find ./$@/lib -name lib$@.so\*`; do ln -fs ../$@/lib/`basename $${lib}` ./lib/; done



# Regular compilcation
#
${objs}: submod ${srcs} ${heads}
	@echo "[+] Compiling objects ..."
	${CXX} ${cppflags} -c $(addprefix ./src/,$(patsubst %.o,%.cc,$(notdir $@))) -o $@
	@echo



# Libraries
#
${lib}: $(objs)
	@echo "[+] Creating shared libraries ..."
	${CXX} -shared -W1,-soname,$(basename $@) ${ldflags} -o $(addprefix ./lib/,$@) ${objs}
	@cd ./lib; ln -fs $@ $(basename $@); ln -fs $(basename $@) $(basename $(basename $@))
	@echo



# Executables
#
${progs}: ${objs} 
	@echo "[+] Compiling programs ..."
	${CXX} ${cppflags} -c src/$@.cpp -o ./obj/$@.o
	${CXX} ${ldflags} $(addprefix ./lib/,${lib}) ./obj/$@.o -o ./bin/bsm_$@
	@echo



# Cleaning
#
cleanbin:
ifneq ($(strip ${progs}),)
	rm -f $(addprefix ./bin/bsm_,${progs})
endif

clean: cleanbin
	rm -f ./obj/*.o
	rm -f $(addprefix ./lib/,$(basename $(basename ${lib}))*)

${clean_mod}:
	${MAKE} -C $(subst CLN_,,$@) clean
	rm -f ./lib/lib$(subst CLN_,,$@).so*
