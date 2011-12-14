#!/usr/bin/env bash

sum_samples()
{
    cuts=5
    for folder in $@
    do
        tail -30 $folder/debug_signal_p150_hlt.log | grep "[+]" | cut -d= -f 3 | awk '{print $3}' | grep -E '[0-9]' | tail -$cuts | xargs -I {} echo {}\ | xargs echo | sed 's/ $/\n/'
    done | awk '{a+=$1; b+=$2; c+=$3; d+=$4; e+=$5; f+=$6} END { print a, b, c, d, e, f}'
}

echo -n "Stop "
sum_samples {s,sa}top_*

echo -n "Wjets "
sum_samples wjets

echo -n "Zjets "
sum_samples zjets

echo -n "TTjets "
sum_samples ttjets

echo -n "Data "
sum_samples golden_*

exit 0
