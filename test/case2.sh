#!/bin/bash

ip=127.0.0.1
port=12345
mkdir -p log/CASE2/
rm -rf log/CASE2/*
echo "Spawining server......"
../bin/server CASE2/Records.txt $ip $port 0 0 > server.log 2>&1 &
SPROC=$!
sleep 3
echo "Spawining client......1"
../bin/client CASE2/Transactions.txt 100 $ip $port 1 1000 1 > client.log 2>&1 &
PROC1_ID=$!

echo "Spawining client......2"
../bin/client CASE2/Transactions.txt 100 $ip $port 1 1000 2 > client.log 2>&1 &
PROC2_ID=$!

echo "Waiting for client to finish"
while kill -0 "$PROC1_ID" >/dev/null 2>&1; do
	    echo "PROCESS IS RUNNING" > /dev/null
done

echo "Waiting for client to finish"
while kill -0 "$PROC2_ID" >/dev/null 2>&1; do
	    echo "PROCESS IS RUNNING" > /dev/null
done

echo "Kill server..."
kill -9 "$SPROC"
mkdir -p log
mv server.log client.log* client_log* log/CASE2/.
echo "Final account balance..."
tail -12 log/CASE2/server.log | cut -d':' -f4,5
