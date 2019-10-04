#!/bin/bash
DATE="$(date +%Y%m%d_%H%M)"
echo $DATE
RUNFILE=DistUeBias_R256
UE=100
PICO=6
PICOVAR=`expr "$UE / $PICO * 0.3" | bc`
LOAD=0.5
SIMTIME=90
ABS=2
TEE=${RUNFILE}_test_run_${DATE}.rslt
PARAM="./waf --run='scratch/$RUNFILE -nUes=$UE -nPicoEnb=$PICO -picoVariance=$PICOVAR -Load=$LOAD -simTime=$SIMTIME -absInterval=$ABS' 2>&1 |tee $TEE"
echo $PARAM
eval $PARAM
