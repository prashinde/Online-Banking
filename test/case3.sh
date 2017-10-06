#!/bin/bash

ip=127.0.0.1
port=123456
rate=1
mult=1000

echo "Spawining server......"
../bin/server CASE3/Records.txt $ip $port 0 0 > server.log 2>&1 &
SPROC=$!

GRAPH_DIR=graph
rm -rf $GRAPH_DIR
mkdir -p $GRAPH_DIR
for i in {1..3};
do
	rm -rf log
	rm client.log
	echo "--Spwaning $i clients...."
	for j in `seq 1 $i`
	do
		../bin/client CASE3/Transactions.txt 100 $ip $port $rate $mult $j > client.log 2>&1 &
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
$(dpkg-query -l gnuplot > /dev/null 2>&1)
r=$?
if [ $r -eq 0 ]
then
gnuplot -e "set title 'Scalability-Response Time Vs. concurrecnt connections';set xlabel'No. Concurrent connections'; set ylabel'Response Time(ms)';plot 'graph.plot' w linespoint, '' with labels; pause -1"
else
	echo "GNUPLOT not installed"
fi
