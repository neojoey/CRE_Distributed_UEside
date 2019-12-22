#!/bin/bash
DATE="$(date +%Y%m%d_%H%M)"
echo $DATE
RUNFILE=disttest
UE=200
PICO=4
PICOVAR=`expr "$UE / $PICO * 0.3" | bc`
LOAD=0.5
SIMTIME=120
ABS=2
SEED=$1
RND=$2
DIST=$3
TYPE=HOTSPOT
TEE=${RUNFILE}${RND}_SEED${SEED}_${TYPE}_PICO${PICO}_UE${UE}_L${LOAD}_A${ABS}_D${DIST}_${DATE}.rslt
PARAM="./waf --run='scratch/$RUNFILE -nUes=$UE -nPicoEnb=$PICO -picoVariance=$PICOVAR -Load=$LOAD -simTime=$SIMTIME -absInterval=$ABS -rndRun=$RND -rndSeed=$SEED -scenarioType=$TYPE -distPico=$DIST' 2>&1 |tee $TEE"
echo $PARAM
eval $PARAM
