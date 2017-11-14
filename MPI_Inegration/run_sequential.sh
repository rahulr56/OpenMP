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
    

make sequential

. params.sh

for intensity in $INTENSITIES;
do
    for n in $NS;
    do
	FILE=${RESULTDIR}/sequential_${n}_${intensity}

	if [ ! -f ${FILE} ]
	then
	    ./sequential 1 0 10 ${n} ${intensity} 2>${RESULTDIR}/sequential_${n}_${intensity}  >/dev/null
	fi
    done
done
