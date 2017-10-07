#!/bin/bash

# Port figyelése a bejövő csatorna megállapításához.
echo /dev/ttyUSB0

# Figyelünk, amíg nem jön rajta valami.
while true; do
        read -n 8 line < /dev/ttyUSB0

        # Ha vége a felderítésnek akkor kilépünk.
        if [[ $line == -1 ]]
        then
                echo "End discovery mode."
		echo $line > /dev/ttyUSB1
		sleep 1
                break
        fi

        # Egyébként növeljük az azonosítót és küldjük tovább.
        id=$((id+1))
        echo $id > /dev/ttyUSB1
	sleep 1
done

echo "Receiver is"
echo /dev/ttyUSB0

echo "ID is "$id

# A másik csatorna figyelésének leállítása.
pkill discover1

# A bejövő port figyelésének indítása.
pkill receiver
../sh/receiver.sh /dev/ttyUSB0 /dev/ttyUSB1 $id &

# Kimenő port bemenetének figyelése.
pkill sender
../sh/sender.sh /dev/ttyUSB1 $id &

