#!/usr/bin/env bash
#
# Compile morphing code
#
# Created by Samvel Khalatyan, Jan 11, 2012
# Copyright 2011, All rights reserved

g++ -I`root-config --incdir` -I/opt/local/include/ `root-config --libs` -lRooStats -lRooFit -lRooFitCore morph_single.cxx -o morph

if [[ 0 != $? ]]
then
    echo "Adjust paths"
    echo g++ -I`root-config --incdir` -I/opt/local/include/ `root-config --libs` -lRooStats -lRooFit -lRooFitCore morph_single.cxx -o morph

    exit 1
fi

exit 0
