#!/bin/bash
FILE_LIST=`ls *Delay*`
OUTFILE='delay_satisfaction.txt'
echo $FILE_LIST
for i in $FILE_LIST
	do
		IDX=`awk '{if ($2 > 4.0e7 && FNR>7) print $1-1}' $i |head -1`
		echo $IDX >> $OUTFILE
	done
cat $OUTFILE
