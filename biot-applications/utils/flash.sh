#bin/sh

if [ $# -eq 0 ]
      then
            echo "need a port number"
            echo "$0 - flash board"
            echo "usage:"
            echo "   $0 n"
            echo "   where n = /dev/ttyACMn port number eg:"
            echo "   $0 2"
            exit 0;
fi
PORT="ttyACM"$1
echo flashing to: $PORT
make list-ttys
ADDR=`make list-ttys | grep $PORT | sed -e "s/.* '//; s/'.*//"`
echo $ADDR
SERIAL=$ADDR make flash
#SERIAL=$ADDR make clean flash
