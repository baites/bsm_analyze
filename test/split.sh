#!/usr/bin/env bash
#
# Split input file into set of files with N lines per file
#
# Created by Samvel Khalatyan, Aug 30, 2011
# Copyright 2011, All rights reserved

if [[ 2 -gt $# ]]
then
    echo Usage: `basename $0` input.txt FILES_PER_SER

    exit 1
fi

input=$1
if [[ !(-r "$input") ]]
then
    echo input file does not exist

    exit 1
fi

files_per_set=$2
files=`wc -l $input | cut -f1 -d' '`
sets=$(( files / files_per_set ))
if [[ 0 -lt $(( files - sets * files_per_set )) ]]
then
    sets=$(( sets + 1 ))
fi

for (( set = 0; sets > set; ++set ))
do
    eval "awk 'NR > $(( set * files_per_set )) && NR <= $(( (set + 1) * files_per_set ))' $input > set$(( set + 1 )).txt"
done

exit 0
