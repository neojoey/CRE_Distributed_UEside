#!/bin/bash
DATE="$(date +%Y%m%d_%H%M)"
echo $DATE
RUNFILE=DistUeBias_R256_RND
UE=120
PICO=6
PICOVAR=`expr "$UE / $PICO * $1" | bc`
LOAD=0.5
SIMTIME=90
ABS=8
RND=1
TEE=${RUNFILE}${RND}_UE${UE}_L${LOAD}_A${ABS}_V${1}_${DATE}.rslt
PARAM="./waf --run='scratch/$RUNFILE -nUes=$UE -nPicoEnb=$PICO -picoVariance=$PICOVAR -Load=$LOAD -simTime=$SIMTIME -absInterval=$ABS -rndRun=$RND' 2>&1 |tee $TEE"
echo $PARAM
eval $PARAM
