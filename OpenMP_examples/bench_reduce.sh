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


N="100000000"
THREADS="1 2 4 8 12 16"

make reduce

for n in $N;
do
    for t in $THREADS;
    do
	./reduce $n $t static -1 >/dev/null 2> ${RESULTDIR}/reduction_${n}_${t}_static_-1
	./reduce $n $t dynamic 1 >/dev/null 2> ${RESULTDIR}/reduction_${n}_${t}_dynamic_1
	./reduce $n $t dynamic 1000 >/dev/null 2> ${RESULTDIR}/reduction_${n}_${t}_dynamic_1000
	./reduce $n $t dynamic 100000 >/dev/null 2> ${RESULTDIR}/reduction_${n}_${t}_dynamic_100000	
    done
done
	     
for n in $N;
do
    for t in $THREADS;
    do
	#output in format "thread seq par"
	echo ${t} \
	     $(cat ${RESULTDIR}/reduction_${n}_1_static_-1) \
	     $(cat ${RESULTDIR}/reduction_${n}_${t}_static_-1)
    done   > ${RESULTDIR}/speedup_reduction_${n}_static_-1

    for t in $THREADS;
    do
	#output in format "thread seq par"
	echo ${t} \
	     $(cat ${RESULTDIR}/reduction_${n}_1_static_-1) \
	     $(cat ${RESULTDIR}/reduction_${n}_${t}_dynamic_1)
    done   > ${RESULTDIR}/speedup_reduction_${n}_dynamic_1

    for t in $THREADS;
    do
	#output in format "thread seq par"
	echo ${t} \
	     $(cat ${RESULTDIR}/reduction_${n}_1_static_-1) \
	     $(cat ${RESULTDIR}/reduction_${n}_${t}_dynamic_1000)
    done   > ${RESULTDIR}/speedup_reduction_${n}_dynamic_1000

    for t in $THREADS;
    do
	#output in format "thread seq par"
	echo ${t} \
	     $(cat ${RESULTDIR}/reduction_${n}_1_static_-1) \
	     $(cat ${RESULTDIR}/reduction_${n}_${t}_dynamic_100000)
    done   > ${RESULTDIR}/speedup_reduction_${n}_dynamic_100000
done

gnuplot <<EOF

set terminal pdf
set output 'reduce_plots.pdf'

set style data linespoints


set key top left;
set xlabel 'threads'; 
set ylabel 'speedup';
set xrange [1:20];
set yrange [0:20];
set title 'n=$N';
plot '${RESULTDIR}/speedup_reduction_${N}_static_-1' u 1:(\$2/\$3) t 'static' lc 1, \
     '${RESULTDIR}/speedup_reduction_${N}_dynamic_1' u 1:(\$2/\$3) t 'dynamic,1' lc 3, \
     '${RESULTDIR}/speedup_reduction_${N}_dynamic_1000' u 1:(\$2/\$3) t 'dynamic,1000' lc 4, \
     '${RESULTDIR}/speedup_reduction_${N}_dynamic_100000' u 1:(\$2/\$3) t 'dynamic,100000' lc 5;

EOF
