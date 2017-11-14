#!/bin/sh

qsub -q mamba -l nodes=1:ppn=16 -d $(pwd) ./run_sequential.sh


