#!/bin/bash

# Nulla azonosító küldése (ezt léptetik majd eggyel és küldik tovább a többiek).
id=0
while true; do
	echo $id > $1
	sleep 5
done
