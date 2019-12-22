#!/bin/bash
FILE_LIST=`ls *Delay*`
OUTFILE='delay_satisfaction.txt'

if test -f "$OUTFILE"; then
	rm -f $OUTFILE
fi

echo $FILE_LIST
for i in $FILE_LIST
	do
		#IDX=`awk 'BEGIN{ cnt=0; zero=0} {if($2==0){ zero+=1 } if ($2 > 4.0e7 && FNR>7) {cnt=$1-1;exit}} END{ printf ("%d, %d\n", zero, cnt)}' $i |head -1`
		IDX=`awk 'BEGIN{ cnt=0; zero=0} {if($2==0){ zero+=1 } if ($2 > 4.0e7 && FNR>7) {cnt=$1-1;exit}} END{ printf ("%d\n", cnt-zero)}' $i |head -1`
		echo $IDX >> $OUTFILE
	done
cat $OUTFILE
