#!/bin/bash

u="$1"
rm -rf ../log/*
for i in `seq 1 $u`
do
	cd ../bin/.
	./client ../test/ctest 100 $2 $3 0 $i &
done
