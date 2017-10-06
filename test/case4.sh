#!/bin/bash

ip=127.0.0.1
port=123456
rate=1
mult=1000

echo "Spawining server......"
../bin/server CASE4/Records.txt $ip $port 0 0 > server.log 2>&1 &
SPROC=$!

GRAPH_DIR=graph
rm -rf $GRAPH_DIR
mkdir -p $GRAPH_DIR
for i in {1..30};
do
	rm -rf log
	rm client.log
	echo "--Spwaning $i clients...."
	for j in `seq 1 40`
	do
		../bin/client CASE4/Transactions.txt 100 $ip $port $i $mult $j > client.log 2>&1 &
		pids+=($!)
	done

	echo "--Wait for $i clients to finish...."
	for pid in "${pids[@]}"; do
		wait "$pid"
		# do something when a job completes
	done
	pids=()
	echo "--Client finished...."
	mkdir -p log
	mv client_log* log/ 
	#rm -rf ../$GRAPH_DIR/*

	avgtime=`echo "0"|bc`
	for file in log/*;
	do 
		line=$(tail -1 $file)
		number=$(echo $line | cut -d':' -f2)
		avgtime=$(echo "scale=8;$avgtime+$number"|bc)
	done

	avgtime=$(echo "scale=8;$avgtime/$i"|bc)
	printf "%d %08.8f\n" "$i" "$avgtime" >> $GRAPH_DIR/graph.plot
done

echo "Kill server..."
kill -9 $SPROC
rm -rf server.log

cd $GRAPH_DIR/.
gnuplot -e "set title 'Scalability-Response Time Vs. Request Rate';set xlabel'Request rate(queries/ms)'; set ylabel'Response Time(ms)';plot 'graph.plot' w linespoint, '' with labels; pause -1"
