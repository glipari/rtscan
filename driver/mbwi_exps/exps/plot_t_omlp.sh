#!/bin/bash

EXP=exp_omlp
GRDIR=graphs_omlp_t

if [[ -d $GRDIR ]]; then echo '$GRDIR exists' 
else mkdir $GRDIR; fi

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
                    let index=1;
                    let nr=s+l
                    let n=nm*m
                    let mn=g*2
                    let mx=g*6
                    if (( "$g" <= "$nr" )) && (( "$n" >= "$mn" )) && (( "$n" <= "$mx" )); then
                        for u in $U; do
                            let index=index+1
                            TFILE=temp${index}.txt
                            echo "#" > $TFILE
                            for t in $T; do
                            # actu=$(float_eval "$m * $u")
                                FNAME=exp_m${m}_n${n}_g${g}_s${s}_l${l}_t${t}/tres_sorted.txt
                                MYFILE=${EXP}/${FNAME}
                                echo $t"," $(head -n $index $MYFILE | tail -1) >> $TFILE
                            done
                            GFILE=tset${index}.gpl
                            # now that I have all elements in temp.txt, I have to plot it
                            FNAME_T=exp_m${m}_n${n}_g${g}_s${s}_l${l}_u${u}
                            echo "set term postscript eps color font \"Times-Roman\" 24 lw 1.6" > $GFILE
                            echo "set output \""$GRDIR/$FNAME_T.eps"\"" >> $GFILE
                            echo "set title \"m="$m " n="$n " g="$g " s="$s " l="$l " u="$u "\"" >> $GFILE 
                            echo "set xlabel \"Threshold\"" >> $GFILE 
                            echo "set ylabel \"N. sched. sets\"" >> $GFILE 
                            echo "set yrange [0:105]" >> $GFILE 
                            echo "set pointsize 1.5" >> $GFILE
                            echo "plot \""$TFILE"\" using 1:6 w lp t \"Feasible\",\""$TFILE"\" using 1:7 w lp t \"M-BWI\",\""$TFILE"\" using 1:8 w lp t \"FMLP\", \""$TFILE"\" using 1:9 w lp t \"OMLP\"" >> $GFILE 
                            gnuplot $GFILE &
                        done
                        wait
                    fi
                done
            done 
        done 
    done
done
