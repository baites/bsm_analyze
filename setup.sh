#!/usr/bin/env bash
#
# Setup the environment for the code to compile. Setup should be run only
# once per session.
#
# Created by Samvel Khalatyan, Aug 04, 2011
# Copyright 2011, All rights reserved

CODE=`pwd`

library_path=DYLD_LIBRARY_PATH
if [[ "Linux" == "`uname`" ]]
then
    if [[ "" == "$CMSSW_BASE" ]]
    then
        echo CMSSW is not set

        library_path=
    else
        library_path=LD_LIBRARY_PATH
    fi
fi

if [[ "" != "$library_path" ]]
then
    cmd="echo \$${library_path} | awk '{print index(\$0, \"${CODE}\")}'"
    index=`eval $cmd`

    if [[ "0" -ne "$index" ]]
    then
        echo "Environment is already set."
    else
        echo "setting the environment"

        export ${library_path}="${!library_path}:${CODE}/lib:${CODE}/test/lib"
        export PATH="${CODE}/bin:${CODE}/test/bin:${PATH}"
        export PYTHONPATH="${CODE}/python:${PYTHONPATH}"

        if [[ "Linux" == "`uname`" ]]
        then
            BOOST_PATH=`echo $CMSSW_FWLITE_INCLUDE_PATH | sed -e 's#:#\n#g' | grep boost`
            BOOST_PATH=`dirname ${BOOST_PATH}`

            export LDFLAGS="${LDFLAGS} -L${BOOST_PATH}/lib"
            export CPPFLAGS="${CPPFLAGS} -I${BOOST_PATH}/include"

            # Setup Proto Buf 2.4.1 [to be enabled in future]
            #
            # PROTOBUF_PATH=`dirname ${BOOST_PATH}`
            # PROTOBUF_PATH=`dirname ${PROTOBUF_PATH}`/protobuf/2.4.1
            #
            # export LDFLAGS="${LDFLAGS} -L${BOOST_PATH}/lib -L${PROTOBUF_PATH}/lib"
            # export CPPFLAGS="${CPPFLAGS} -I${BOOST_PATH}/include -I${PROTOBUF_PATH}/include"
            # export ${library_path}="${PROTOBUF_PATH}/lib:${!library_path}"
            # export PATH="${PROTOBUF_PATH}/bin:$PATH"
        fi
    fi
fi

export PATH=/uscms/home/baites/nobackup/Utils/python/3.2.2/bin:$PATH
