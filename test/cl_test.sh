#!/bin/bash

ip=$1
port=$2
rate=$3

GRAPH_DIR=graph
rm -rf ../$GRAPH_DIR
for i in {1..4};
do
	rm -rf ../log/*
	for j in `seq 1 $i`
	do
		cd ../bin/.
		./client ../test/ctest 100 $1 $2 $3 $j &
	done

	wait

	cd ../log/.
	mkdir -p ../$GRAPH_DIR
	#rm -rf ../$GRAPH_DIR/*

	avgtime=0.0
	for file in ../log/*;
	do 
		line=$(tail -1 $file)
		number=$(echo $line | cut -d':' -f2)
		echo $number
		avgtime=$(echo "scale=8;$avgtime+$number"|bc)
	done

	avgtime=$(echo "scale=8;$avgtime/$i"|bc)
	printf "%d %08.8f\n" "$i" "$avgtime" >> ../$GRAPH_DIR/graph.plot
	cd ../$GRAPH_DIR/.
done
gnuplot -e "plot 'graph.plot' with lines; pause -1"
