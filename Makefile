# Set CPPFLAGS environment variable for debugging:
#
# 	export CPPFLAGS=-gdwarf-2
#
# Created by Samvel Khalatyan, Aug 03, 2011
# Copyright 2011, All rights reserved

CXX ?= g++

# Subsystems that have compilable libraries
#
submod = bsm_core bsm_input bsm_stat JetMETObjects
lib_submod = $(patsubst %,lib/lib%.so,${submod})
lib = ./lib/libbsm_analyze.so.1.3

# Get list of all heads, sources and objects. Each source (%.cc) whould have
# an object file
#
heads = $(wildcard ./interface/*.h)
srcs = $(wildcard ./src/*.cc)

objs = $(foreach obj,${srcs},$(addprefix ./obj/,$(patsubst %.cc,%.o,$(notdir ${obj}))))

# List of programs with main functions to be filtered out of objects
#
progs = $(foreach prog,$(wildcard ./src/*.cpp),$(addprefix ./bin/bsm_,$(patsubst ./src/%.cpp,%,${prog})))

# Special rules to clean each subsystem
#
clean_mod = $(addprefix CLN_,${submod})

# Need to operate with copies of the env variables b/c the latter ones are
# passed to sumbodules and shoould remain the same
#
cppflags = ${CPPFLAGS} ${debug} -fPIC -pipe -Wall -I./ -I$(shell root-config --incdir) -DSTANDALONE -I./bsm_input/message
ldflags = ${LDFLAGS} $(shell root-config --libs) -L./lib $(foreach mod,${submod},$(addprefix -l,${mod})) -lboost_filesystem -lboost_system -lboost_program_options -lboost_regex -lprotobuf
ifeq ($(shell uname),Linux)
	ldflags  += -L/usr/lib64 -lboost_thread
else
	cppflags += -I/opt/local/include
	ldflags  += -L/opt/local/lib -lboost_thread-mt
endif

# Rules to be always executed: empty ones
#
.PHONY: prog

lib: ${lib}

all: prog

submod: ${lib_submod}

obj: ${objs}

prog: ${progs}

cleanall: clean ${clean_mod}



# Compile modules
#
${lib_submod}:
	$(eval submodule=$(patsubst lib/lib%.so,%,$@))
	@echo "[+] Compiling sub-module ${submodule} ..."
	${MAKE} -C ${submodule}
	@if [[ "JetMETObjects" != ${submodule} ]]; then ln -fs ../${submodule}/lib/lib${submodule}.so $@; fi
	@if [[ "JetMETObjects" != ${submodule} ]]; then for lib in `find ${submodule}/lib -name lib${submodule}.so\*`; do ln -fs ../${submodule}/lib/`basename $${lib}` ./lib/; done fi



# Regular compilcation
#
${objs}: obj/%.o: src/%.cc interface/%.h ${lib_submod}
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
${progs}: bin/bsm_%: src/%.cpp ${lib}
	@echo "[+] Compiling programs $@ ..."
	$(eval prog_name=$(patsubst bin/bsm_%,%,$@))
	${CXX} ${cppflags} -c src/${prog_name}.cpp -o ./obj/${prog_name}.o
	${CXX} ${ldflags} ${lib} ./obj/${prog_name}.o -o $@
	@echo



# Cleaning
#
cleanbin:
ifneq ($(strip ${progs}),)
	rm -f ./bin/bsm_*
endif

clean: cleanbin
	rm -f ./obj/*.o
	rm -f ./lib/*

${clean_mod}:
	${MAKE} -C $(subst CLN_,,$@) clean
	rm -f ./lib/lib$(subst CLN_,,$@).so*
