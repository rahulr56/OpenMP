#!/bin/sh

RESULTDIR=result/
h=`hostname`

if [ "$h" = "mba-i1.uncc.edu"  ];
then
    echo Do not run this on the headnode of the cluster, use qsub!
    exit 1
fi

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi


N="1000000000"
THREADS="1 2 4 8 12 16"

make prefixsum prefixsum_seq

#TODO prefix sum should really run against a true sequential code.

for n in $N;
do
    ./prefixsum_seq $n  >/dev/null 2> ${RESULTDIR}/prefixsum_${n}
    for t in $THREADS;
    do
    ./prefixsum $n $t  >/dev/null 2> ${RESULTDIR}/prefixsum_${n}_${t}
    done
done
         
for n in $N;
do
    for t in $THREADS;
    do
    #output in format "thread seq par"
    echo ${t} \
         $(cat ${RESULTDIR}/prefixsum_${n}) \
         $(cat ${RESULTDIR}/prefixsum_${n}_${t})
    done   > ${RESULTDIR}/speedup_prefixsum_${n}
done

gnuplot <<EOF

set terminal pdf
set output 'prefixsum_plots.pdf'

set style data linespoints


set key top left;
set xlabel 'threads'; 
set ylabel 'speedup';
set xrange [1:20];
set yrange [0:20];
set title 'n=$N';
plot '${RESULTDIR}/speedup_prefixsum_${N}' u 1:(\$2/\$3) t 'prefixsum'


EOF
