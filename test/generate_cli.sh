#!/bin/bash

#a1=`cut -d " " -f1 ./f`; echo $a1;
#a2=`cut -d " " -f2 ./f`; echo $a2;
#a3=`cut -d " " -f3 ./f`; echo $a3;
#a4=`cut -d " " -f4 ./f`; echo $a4;

for i in {1..10000}
do
	td=`shuf -i 1-5 -n 1`;
	a1=$((a1+td));

	acc=`shuf -i 100-10000 -n 1`;

	optype=`shuf -i 0-1 -n 1`;
	if [ $optype == 0 ]
	then
		ch="w";
	else
		ch="d";
	fi

	amount=`shuf -i 0-1000 -n 1`;

	echo "$a1 $acc $ch $amount" >> trans.txt
done
