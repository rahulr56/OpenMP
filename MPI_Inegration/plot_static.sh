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
	
	    FILE=${RESULTDIR}/static_${N}_${INTENSITY}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		echo missing static result file "${FILE}". Have you run queue_static and waited for completion?
	    fi

	    partime=$(cat ${RESULTDIR}/static_${N}_${INTENSITY}_${PROC})
	    
	    echo ${PROC} ${seqtime} ${partime}
	done > ${RESULTDIR}/speedup_static_ni_${N}_${INTENSITY}


	GNUPLOTSTRONG="${GNUPLOTSTRONG} set title 'strong scaling. n=${N} i=${INTENSITY}'; plot '${RESULTDIR}/speedup_static_ni_${N}_${INTENSITY}' u 1:(\$2/\$3);"
    done
done

#weak scaling

for INTENSITY in ${INTENSITIES};
do
    for N in ${NS};
    do
	
	for PROC in ${PROCS}
	do
	    REALN=$( echo ${N} \* ${PROC}  | bc)
	    
	    FILE=${RESULTDIR}/static_${REALN}_${INTENSITY}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		echo missing static result file "${FILE}". Have you run queue_static and waited for completion?
	    fi

	    partime=$(cat ${RESULTDIR}/static_${REALN}_${INTENSITY}_${PROC})
	    
	    echo ${PROC} ${partime}
	done > ${RESULTDIR}/time_static_ni_${N}_${INTENSITY}


	GNUPLOTWEAK="${GNUPLOTWEAK} set title 'weak scaling. n=${N} i=${INTENSITY}'; plot '${RESULTDIR}/time_static_ni_${N}_${INTENSITY}' u 1:2;"
    done
done


gnuplot <<EOF
set terminal pdf
set output 'static_sched_plots.pdf'

set style data linespoints

set key top left

set xlabel 'Proc'
set ylabel 'Speedup'

${GNUPLOTSTRONG}

set xlabel 'Proc'
set ylabel 'Time (in s)'

${GNUPLOTWEAK}


EOF
