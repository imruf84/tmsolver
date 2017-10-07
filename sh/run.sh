#!/bin/sh

# Portok beállítása.
stty -F /dev/ttyUSB0 115200 cs8 -cooked -cstopb -parenb
stty -F /dev/ttyUSB1 115200 cs8 -cooked -cstopb -parenb

# Kommunikációs csatornák felderítése.
pkill discover
pkill receiver
pkill sender
echo "Waitin for ID from"
../sh/discover0.sh &
../sh/discover1.sh &
