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

	for PROC in ${PROCS}
	do
	
	    FILE=${RESULTDIR}/dynamic_${N}_${INTENSITY}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		qsub -d $(pwd) -q mamba -l procs=${PROC} -v FID=1,A=0,B=10,N=${N},INTENSITY=${INTENSITY},PROC=${PROC} ./run_dynamic.sh
	    fi

	done

    done
done


