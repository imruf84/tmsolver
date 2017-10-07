#!/bin/sh

# Portok beállítása.
stty -F /dev/ttyUSB0 115200 cs8 -cooked -cstopb -parenb
stty -F /dev/ttyUSB1 115200 cs8 -cooked -cstopb -parenb

# Kommunikációs csatornák bezárása.
pkill receiver_server
pkill sender_server

#Utolsó node üzeneteinek a figyelése.
pkill id_receiver
../sh/id_receiver.sh /dev/ttyUSB1 /dev/ttyUSB0 &

# Azonosító küldésének megkezdése.
pkill id_sender
../sh/id_sender.sh /dev/ttyUSB0 &
