#!/usr/bin/env bash

filename=$1
sum_samples()
{
    for pattern in $@
    do
        if [[ "qcd_data" != "$pattern" ]]
        then
            grep -E "^$pattern" $filename
        else
            echo `grep -E "^$pattern" $filename | awk 'END {print $1, $2, $3, $4, $6, $5, $7}'`
        fi
    done | awk '{a+=$2; b+=$3; c+=$4; d+=$5; e+=$6; f+=$7} END {printf("\$%.0f \\pm %.0f\$ & \$%.0f \\pm %.0f\$ & \$%.0f \\pm %.0f\$ & \$%.0f \\pm %.0f\$ & \$%.0f \\pm %.0f\$ \\\\\n", a, sqrt(a), b, sqrt(b), c, sqrt(c), d, sqrt(d), e, sqrt(e))}'
}

sum_samples_no_error()
{
    for pattern in $@
    do
        if [[ "qcd_data" != "$pattern" ]]
        then
            grep -E "^$pattern" $filename
        else
            echo `grep -E "^$pattern" $filename | awk 'END {print $1, $2, $3, $4, $6, $5, $7}'`
        fi
    done | awk '{a+=$2; b+=$3; c+=$4; d+=$5; e+=$6; f+=$7} END {printf("\$%.0f\$ & \$%.0f\$ & \$%.0f\$ & \$%.0f\$ & \$%.0f\$ \\\\\n", a, b, c, d, e)}'
}

for input in zprime_m{1,2,3}*
do
    echo -n "\$Z^\\prime\$, \$M="
    if [[ "zprime_m1000_w10" == "$input" ]]
    then
        echo -n "1"
    elif [[ "zprime_m1500_w15" == "$input" ]]
    then
        echo -n "1.5"
    elif [[ "zprime_m2000_w20" == "$input" ]]
    then
        echo -n "2"
    elif [[ "zprime_m3000_w30" == "$input" ]]
    then
        echo -n "3"
    else
        echo -n "XYZ"
    fi
    echo -n "\\TeVcc\$ & "

    sum_samples "$input"
done
echo \\hline

echo -n "Single top & "
sum_samples "stop" satop

for input in {w,z,tt}jets
do
    if [[ "wjets" == "$input" ]]
    then
        echo -n "\$W\$+jets & "
    elif [[ "zjets" == "$input" ]]
    then
        echo -n "\$Z\$+jets & "
    elif [[ "ttjets" == "$input" ]]
    then
        echo -n "\$t\\bar{t}\$ & "
    else
        echo -n "$input & "
    fi

    sum_samples "$input"
done

echo \\hline

echo -n "Total MC & "
sum_samples "stop" satop {w,z,tt}jets

echo -n "QCD data-driven & "
sum_samples qcd_data

echo \\hline

echo -n "Total background & "
sum_samples "stop" satop {w,z,tt}jets qcd_data

echo -n "Data 2011 & "
sum_samples_no_error data

echo \\hline

exit 0
