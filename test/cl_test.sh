#!/bin/bash

u="$1"
rm -rf ../log/*
for i in `seq 1 $u`
do
	cd ../bin/.
	./client ../test/ctest 100 127.0.0.1 12345 0 $i &
done
