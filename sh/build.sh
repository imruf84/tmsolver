#!/bin/sh

pkill oled
cd ../projects/oled/
make
cp ./oled ../../bin/
mkfifo ../../bin/oledfifo
../../bin/oled < ../../bin/oledfifo &

pkill solver
cd ../solver/
cbp2make -in solver.cbp -out Makefile
make

mkfifo ../../bin/senderfifo
