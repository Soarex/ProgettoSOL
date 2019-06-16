#!/bin/bash

echo "" > testout.log

for i in {0..49}; do 
		./test_client.out client_$i 1 >> testout.log &
        pids[${i}]=$!
done

for pid in ${pids[*]}; do
    wait $pid
done 

for i in {0..29}; do 
		./test_client.out client_$i 2 >> testout.log &
        pids[${i}]=$!
done

for i in {30..49}; do 
		./test_client.out client_$i 3 >> testout.log &
        pids[${i}]=$!
done
