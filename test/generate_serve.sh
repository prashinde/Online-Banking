#!/bin/bash

#a1=`cut -d " " -f1 ./f`; echo $a1;
#a2=`cut -d " " -f2 ./f`; echo $a2;
#a3=`cut -d " " -f3 ./f`; echo $a3;
#a4=`cut -d " " -f4 ./f`; echo $a4;

for i in {1..3}
do
	n=`./names | cut -d " " -f1`;

	acc=`shuf -i 100-10000 -n 1`;

	amount=`shuf -i 0-1000 -n 1`;

	echo "$acc $n $amount";
done
