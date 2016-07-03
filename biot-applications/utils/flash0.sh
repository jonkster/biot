#bin/sh

PORT="ttyACM0"
echo $PORT
make list-ttys
ADDR=`make list-ttys | grep $PORT | sed -e "s/.* '//; s/'.*//"`
echo $ADDR
#SERIAL=$ADDR make clean flash
SERIAL=$ADDR make flash
