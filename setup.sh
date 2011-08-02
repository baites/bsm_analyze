#!/usr/bin/env bash

CODE=`pwd`
cmd="echo $DYLD_LIBRARY_PATH | awk '{print index(\$0, \"${CODE}\")}'"
index=`eval $cmd`

if [[ "0" -ne "$index" ]]
then
    echo "Environment is already set."
else
    echo "Setting environment."

    export DYLD_LIBRARY_PATH="${DYLD_LIBRARY_PATH}:${CODE}/lib"
    export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${CODE}/lib"

    export PATH=${CODE}/bin:${PATH}
fi
