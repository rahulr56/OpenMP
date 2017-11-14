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
    
mpirun ./static_sched ${FID} ${A} ${B} ${N} ${INTENSITY} 2> ${RESULTDIR}/static_${N}_${INTENSITY}_${PROC}  >/dev/null

