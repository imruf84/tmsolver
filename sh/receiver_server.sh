#!/bin/bash

echo "Receiving from"
echo $1

while true; do

        read -n 8 line < $1
        echo "$line"

done

