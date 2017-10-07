#!/bin/bash

i=0

while true; do
	echo "echo $i" > ../bin/senderfifo
	i=$((i+1))
	sleep 1
done
