#!/bin/bash

# Letiltjuk automatikusan a szóközök trimmelését.
IFS=''
# Parancs vége karakter.
EOC='$'
# Cím elválasztó karakter.
AMD='@'

while true; do

	while read -n 4 c;
	do
		echo "0$AMD$c" > $1
		sleep 0.2
	done  < ../bin/senderfifo

	# Végjel küldése.
	echo "0@$EOC" > $1
done

