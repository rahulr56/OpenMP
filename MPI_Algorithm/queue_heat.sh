#!/bin/sh

. ./params.sh

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

#strong scaling

for N in ${NS};
do

    for PROC in ${PROCS}
    do

        FILE=${RESULTDIR}/heat_strong_${N}_${PROC}

        if [ ! -f ${FILE} ]
        then
            echo "qsub -d $(pwd) -q mamba -l procs=${PROC} -v N=${N},PROC=${PROC} ./run_heat.sh"
            qsub -d $(pwd) -q mamba -l procs=${PROC} -l mem=120GB -v N=${N},PROC=${PROC} ./run_heat.sh
        fi
    done
done


#weak scaling

for N in ${NS};
do
    for PROC in ${PROCS}
    do
        REALN=$( echo ${N} \* ${PROC}  | bc)

        FILE=${RESULTDIR}/heat_weak_${REALN}_${PROC}

        if [ ! -f ${FILE} ]
        then
            echo "qsub -d $(pwd) -q mamba -l procs=${PROC} -v N=${REALN},PROC=${PROC} ./run_heat.sh"
            qsub -d $(pwd) -q mamba -l procs=${PROC} -l mem=120GB -v N=${REALN},PROC=${PROC} ./run_heat.sh
        fi
    done
done


