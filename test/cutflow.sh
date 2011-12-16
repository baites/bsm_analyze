#!/usr/bin/env bash

filename=debug_signal_p150_hlt.log
sum_samples()
{
    cuts=5
    for folder in $@
    do
        tail -30 $folder/$filename | grep "[+]" | sed 's/^.*= //' | awk '{print $3}' | grep -E '[0-9]' | tail -$cuts | xargs -I {} echo {}\ | xargs echo | sed 's/ $/\n/'
    done | awk '{a+=$1; b+=$2; c+=$3; d+=$4; e+=$5; f+=$6} END { print a, b, c, d, e, f}'
}

for folder in {s,sa}top_* {w,z,tt}jets zprime_m{1,2,3}*
do
    echo -n "$folder "
    sum_samples $folder
done

filename=debug_signal_p150_qcd.log

echo -n "qcd_data "
sum_samples golden_*
filename=debug_signal_p150_hlt.log

echo -n "data "
sum_samples golden_*

exit 0