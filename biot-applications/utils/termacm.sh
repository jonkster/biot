#!/bin/sh

if [ $# -eq 0 ]
      then
            echo "need a port number"
            echo "$0 - connect to terminal on board"
            echo "usage:"
            echo "   $0 n"
            echo "   where n = /dev/ttyACMn port number eg:"
            echo "   $0 2"
            exit 0;
fi
minicom -D/dev/ttyACM$1 riotos
