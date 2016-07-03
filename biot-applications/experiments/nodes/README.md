*Networking Project*

# Overview

Rough code for 6lowPAN nodes.

It experiments with udp wireless communication between samr21z-llk boards.

See ../adc/README.md for details on architecture etc

# summary:

1. connect one node with FTDI cable to PC USB.  This will be the edge
   router/root of the DODAG

2. hit reset on both boards

3. hit SW0 on the edge router board

4. from one of the boards, send a udp message to another board to change led
   colour from the shell eg

    usb affe::2 red

The targetted board should change led colour and also fire a return message to
change led colour on the original board (which will respond in kind - basically
like a UNIX 'finger war' but with changing colours :)

