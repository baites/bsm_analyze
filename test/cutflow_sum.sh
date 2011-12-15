#!/usr/bin/env bash

filename=$1
sum_samples()
{
    for pattern in $@
    do
        grep $pattern $filename
    done | awk '{a+=$2; b+=$3; c+=$4; d+=$5; e+=$6; f+=$7} END {print a, "&", b, "&", c, "&", d, "&", e, "\\\\"}'
}

echo -n "stop & "
sum_samples "stop" satop

for input in {w,z,tt}jets
do
    echo -n "$input & "
    sum_samples "$input"
done

echo -n "total & "
sum_samples "stop" satop {w,z,tt}jets

echo -n "data & "
sum_samples data

for input in zprime_m{1,2,3}*
do
    echo -n "$input & "
    sum_samples "$input"
done

exit 0
