#!/bin/sh

RESULTDIR=result/
h=`hostname`

#if [ "$h" = "mba-i1.uncc.edu"  ];
#then
#    echo Do not run this on the headnode of the cluster, use qsub!
#    exit 1
#fi

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi


NS="1000 100000000"
THREADS="1 2 4 8 12 16"
GRANS="1 1000"
INTENSITIES="10 1000"

make numint numint_seq

for n in $NS;
do
    for gran in ${GRANS}
    do
	for inten in ${INTENSITIES}
	do

	    ./numint_seq 1 1 10 ${n} ${inten}  >/dev/null 2> ${RESULTDIR}/numint_${n}

	    for t in $THREADS;
	    do
		./numint 1 1 10 ${n} ${inten} $t dynamic ${gran} >/dev/null 2> ${RESULTDIR}/numint_${n}_${t}_${gran}_${inten}
	    done
	done
    done
done

for n in $NS;
do
    for gran in ${GRANS}
    do
	for inten in ${INTENSITIES}
	do
	    for t in $THREADS;
	    do
		#output in format "thread seq par"
		echo ${t} \
		     $(cat ${RESULTDIR}/numint_${n}) \
		     $(cat ${RESULTDIR}/numint_${n}_${t}_${gran}_${inten})
	    done   > ${RESULTDIR}/speedup_numint_${n}_${gran}_${inten}
	done
    done
done

gnuplot <<EOF

set terminal pdf
set output '1_numint_plots.pdf'

set style data linespoints


set key top left;
set xlabel 'threads';
set ylabel 'speedup';
set xrange [1:20];
set yrange [0:99];
set title 'n=1000' ;
plot '${RESULTDIR}/speedup_numint_1000_1_10' u 1:(\$2/\$3) t 'gran=1 inten=10' lc 1, \
     '${RESULTDIR}/speedup_numint_1000_1_1000' u 1:(\$2/\$3) t 'gran=1 inten=1000' lc 3, \
     '${RESULTDIR}/speedup_numint_1000_1000_10' u 1:(\$2/\$3) t 'gran=1000 inten=10' lc 4, \
     '${RESULTDIR}/speedup_numint_1000_1000_1000' u 1:(\$2/\$3) t 'gran=1000 inten=1000' lc 5

set title 'n=100000000' ;
plot '${RESULTDIR}/speedup_numint_100000000_1_10' u 1:(\$2/\$3) t 'gran=1 inten=10' lc 1, \
     '${RESULTDIR}/speedup_numint_100000000_1_1000' u 1:(\$2/\$3) t 'gran=1 inten=1000' lc 3, \
     '${RESULTDIR}/speedup_numint_100000000_1000_10' u 1:(\$2/\$3) t 'gran=1000 inten=10' lc 4, \
     '${RESULTDIR}/speedup_numint_100000000_1000_1000' u 1:(\$2/\$3) t 'gran=1000 inten=1000' lc 5


EOF
