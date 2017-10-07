#!/bin/bash

# Figyelünk, amíg nem jön rajta valami.
while true; do
        read -n 8 id < $1
        if [[ -n "${id/[ ]*\n/}" ]]
        then
                break
        fi
done

# Ha kaptunk választ az utolsó node-tól akkor kezdődhet a munka.

# ID küldő leállítása.
pkill id_sender
sleep 10

# Kommunikációs csatornák bezárása.
pkill receiver_server
pkill sender_server

# Kiírjuk a node-ok számát.
echo $id" nodes found"

# Discovery mód leállításának küldése.
echo -1 > $2
sleep 1

# Kommunikációs csatornák megnyitása.
../sh/receiver_server.sh $1 &
../sh/sender_server.sh $2 &

# Várunk egy kicsit.
sleep 10

# Teszt indítása.
../sh/test.sh &
