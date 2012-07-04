#!/bin/bash

EXP=exp_lower_c

if [[ -d $EXP ]]; then echo '$EXP exists' 
else mkdir $EXP; fi

function float_eval()
{
    local stat=0
    local result=0.0
    if [[ $# -gt 0 ]]; then
        result=$(echo "scale=2; $*" | bc -q 2>/dev/null)
        stat=$?
        if [[ $stat -eq 0 && -z "$result" ]]; then stat=1; fi
    fi
    echo $result
    return $stat
}

M="2 4 6"
N_M="2 3"
G="1 2 3"
S="2 4 6"
L="0 2"
U="0.28 0.32 0.36 0.40 0.44 0.48 0.52 0.56 0.60 0.64 0.68 0.72"
T="0.05 0.10 0.15 0.20 0.25 0.30 0.35 0.40 0.45 0.50"

for m in $M; do
    for nm in ${N_M}; do
        for g in $G; do
            for s in $S; do
                for l in $L; do
                    for t in $T; do
                        let nr=s+l
                        let n=nm*m
                        let mn=g*2
                        let mx=g*6
                        if (( "$g" <= "$nr" )) && (( "$n" >= "$mn" )) && (( "$n" <= "$mx" )); then 
                            MYDIR=${EXP}/exp_m${m}_n${n}_g${g}_s${s}_l${l}_t${t}
                            mkdir $MYDIR
                            for u in $U; do
                                actu=$(float_eval "$m * $u")
                                echo $MYDIR ", u=" $actu ", l=" $l ", s=" $s ", g=" $g ", n=" $n ", m=" $m ", t=" $t
                                ./taskgen.py -u ${actu} -p 50 -q 1000 -g 50 -d logunif -s 100 -n ${n} > ${MYDIR}/tset_u${actu}.txt&
                            done
                            wait 
                            for u in $U; do
                                actu=$(float_eval "$m * $u")
                                seed=$RANDOM
                                let "seed %= 5000"
                                echo $MYDIR ", u=" $actu ", l=" $l ", s=" $s ", g=" $g ", n=" $n ", m=" $m ", t=" $t ", seed=" $seed
                                ../mbwi_simul -r ${seed} -f ${MYDIR}/tset_u${actu}.txt -o ${MYDIR}/tres.txt -m ${m} -s ${s} -l ${l} -1 .9 -2 .4 -3 .1 -z .4 -g ${g} -t ${t} > ${MYDIR}/out_u${actu}.txt &
                            done
                            wait
                            sort ${MYDIR}/tres.txt > ${MYDIR}/tres_sorted.txt 
                        else 
                            echo "too many groups: g =" $g "s =" $s "l =" $l "n =" $n 
                        fi
                    done
                done 
            done
        done
    done
done

                        
                    
