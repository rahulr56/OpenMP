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


N="900000000"
THREADS="1 2 3 4 6 8 10 12 16"

make mergesort

for n in $N;
do
    for t in $THREADS;
    do
    ./mergesort $n $t >/dev/null 2> ${RESULTDIR}/mergesort_${n}_${t}
    done
done

for n in $N;
do
    for t in $THREADS;
    do
    #output in format "thread seq par"
    echo ${t} \
         $(cat ${RESULTDIR}/mergesort_${n}_1) \
         $(cat ${RESULTDIR}/mergesort_${n}_${t})
    done   > ${RESULTDIR}/speedup_mergesort_${n}
done

gnuplot <<EOF

set terminal pdf
set output 'mergesort_plots${N}.pdf'

set style data linespoints


set key top left;
set xlabel 'threads';
set ylabel 'speedup';
set xrange [1:20];
set yrange [0:20];
set title 'n=$N';
plot '${RESULTDIR}/speedup_mergesort_${N}' u 1:(\$2/\$3) t 'merge sort' lc 1, \

EOF
