#!/bin/sh
set -x

RESULTDIR=result
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


N="10 100 1000 10000 100000 1000000 10000000 100000000 1000000000"
# THREADS="1 2 3 4 6 8 10 12 16"
THREADS="16"

make LCS

for n in $N;
do
    for t in $THREADS;
    do
    ./LCS $n $n $t >/dev/null 2> ${RESULTDIR}/LCS_${n}_${t}
    ./LCS $n $n 1 >/dev/null 2> ${RESULTDIR}/LCS_${n}_1
    done
done

for t in $THREADS;
do
    for n in $N;
    do
    #output in format "thread seq par"
    echo ${n} \
         $(cat ${RESULTDIR}/LCS_${n}_1) \
         $(cat ${RESULTDIR}/LCS_${n}_${t})
    done   > ${RESULTDIR}/speedup_LCS_${t}
done

gnuplot <<EOF

set terminal pdf
set output 'LCS_plots${THREADS}.pdf'

set style data linespoints


set key top left;
set xlabel 'n';
set ylabel 'speedup';
set xrange [1:200];
set yrange [0:20];
set title 'n=$N';
plot '${RESULTDIR}/speedup_LCS_${THREADS}' u 1:(\$2/\$3) t 'LCS' lc 1, \

EOF
