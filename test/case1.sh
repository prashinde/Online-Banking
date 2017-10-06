#!/bin/bash

ip=127.0.0.1
port=12345
mkdir -p log/CASE1/
rm -rf log/CASE1/*
echo "Spawining server......"
../bin/server CASE1/Records.txt $ip $port 0 0 > server.log 2>&1 &
SPROC=$!
sleep 3
echo "Spawining client......"
../bin/client CASE1/Transactions.txt 100 $ip $port 1 1000 1 > client.log 2>&1 &
PROC_ID=$!

echo "Waiting for client to finish"
while kill -0 "$PROC_ID" >/dev/null 2>&1; do
	    echo "PROCESS IS RUNNING" > /dev/null
done

echo "Kill server..."
kill -9 "$SPROC"
mkdir -p log
mv server.log client.log* client_log* log/CASE1/.
echo "Final account balance..."
tail -6 log/CASE1/server.log | cut -d':' -f4,5
