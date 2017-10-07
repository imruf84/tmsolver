#!/bin/bash

# Felderítés kilövése.
pkill discover

# Letiltjuk automatikusan a szóközök trimmelését.
IFS=''
# Parancs vége karakter.
EOC='$'
# Cím elválasztó karakter.
AMD='@'


# Azonosító tárolása.
id=$2

while true; do

	while read -n 4 c;
        do
                echo "$id$AMD$c" > $1
                sleep 0.2
        done  < ../bin/senderfifo

        # Végjel küldése.
        echo "$id@$EOC" > $1


done
