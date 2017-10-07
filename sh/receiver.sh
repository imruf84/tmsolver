#!/bin/bash

# Felderítés kilövése.
pkill discover

# Azonosító tárolása.
id=$3

echo "Receiving from"
echo $1

# Letiltjuk automatikusan a szóközök trimmelését.
IFS=''
# Parancs vége karakter.
EOC='$'
# Cím elválasztó karakter.
AMD='@'

while true; do

        read -n 8 line < $1

	# Cím kinyerése.
	address="$(cut -d$AMD -f1 <<<"$line")"
	msg="$(cut -d$AMD -f2 <<<"$line")"

	# Üzenet hozzáfűzése a parancshoz.
	command+=$msg

	# Ha parancs vége karakterrel zárult az üzenet, akkor végrehajtjuk a prancsot.
	if [[ "${line:${#line}-1}" == $EOC ]]; then
		command="$(cut -d$EOC -f1 <<<"$command")"
    		eval $command
		command=""
	fi

done
