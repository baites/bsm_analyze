#!/usr/bin/env bash

CODE=`pwd`

library_path=$DYLD_LIBRARY_PATH
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

        export ${library_path}="${!library_path}:${CODE}/lib"
        export PATH=${CODE}/bin:${PATH}

        if [[ "Linux" == "`uname`" ]]
        then
            BOOST_PATH=`echo $CMSSW_FWLITE_INCLUDE_PATH | sed -e 's#:#\n#g' | grep boost`
            BOOST_PATH=`dirname ${BOOST_PATH}`

            export LDFLAGS="${LDFLAGS} -L${BOOST_PATH}/lib"
            export CPPFLAGS="${CPPFLAGS} -I${BOOST_PATH}/include"
        fi
    fi
fi
