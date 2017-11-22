#!/bin/sh

. ./params.sh

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

#strong scaling

for INTENSITY in ${INTENSITIES};
do
    for N in ${NS};
    do
	FILE=${RESULTDIR}/sequential_${N}_${INTENSITY}
	if [ ! -f ${FILE} ]
	then
	    echo missing sequential result file "${FILE}". Have you run queue_sequential and waited for completion?
	fi

	seqtime=$(cat ${RESULTDIR}/sequential_${N}_${INTENSITY})
	
	for PROC in ${PROCS}
	do
	
	    FILE=${RESULTDIR}/dynamic_${N}_${INTENSITY}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		echo missing dynamic result file "${FILE}". Have you run queue_dynamic and waited for completion?
	    fi

	    partime=$(cat ${RESULTDIR}/dynamic_${N}_${INTENSITY}_${PROC})
	    
	    echo ${PROC} ${seqtime} ${partime}
	done > ${RESULTDIR}/speedup_dynamic_ni_${N}_${INTENSITY}


	GNUPLOTSTRONG="${GNUPLOTSTRONG} set title 'strong scaling. n=${N} i=${INTENSITY}'; plot '${RESULTDIR}/speedup_dynamic_ni_${N}_${INTENSITY}' u 1:(\$2/\$3);"
    done
done

gnuplot <<EOF
set terminal pdf
set output 'dynamic_sched_plots.pdf'

set style data linespoints

set key top left

set xlabel 'proc'
set ylabel 'speedup'

${GNUPLOTSTRONG}

EOF
