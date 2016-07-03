#bin/sh
PORT="ttyACM1"
echo $PORT
make list-ttys
ADDR=`make list-ttys | grep $PORT | sed -e "s/.* '//; s/'.*//"`
echo $ADDR
SERIAL=$ADDR make flash
#SERIAL=$ADDR make clean flash
