#!/usr/bin/env bash

CODE=`pwd`

library_path=DYLD_LIBRARY_PATH
if [[ "Linux" == "`uname`" ]]
then
    library_path=LD_LIBRARY_PATH
fi

cmd="echo \$${library_path} | awk '{print index(\$0, \"${CODE}\")}'"
index=`eval $cmd`

if [[ "0" -ne "$index" ]]
then
    echo "Environment is already set."
else
    echo "setting the environment"

    export ${library_path}="${!library_path}:${CODE}/lib"
    export PATH=${CODE}/bin:${PATH}
fi
