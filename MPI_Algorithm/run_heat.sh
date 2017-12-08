#!/bin/sh

RESULTDIR=result
h=`hostname`

make heat

if [ "$h" = "mba-i1.uncc.edu"  ];
then
    echo Do not run this on the headnode of the cluster, use qsub!
    exit 1
fi

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

mpirun ./heat ${N} 2> ${RESULTDIR}/heat_${N}_${PROC}  >/dev/null

