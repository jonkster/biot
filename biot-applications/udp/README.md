*This is a very experimental setup.  Code is ugly.*

# Overview

Rough code for 6lowPAN nodes.

It experiments with udp wireless communication between samr21z-llk boards.

See ../adc/REDAME.md for details on architecture etc

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
like a UNIX 'finger war'.





Some things that may help

If you have multiple USB connections to different boards:
$ make list-ttys
/sys/bus/usb/devices/3-2: FTDI TTL232R-3V3 serial: 'FTHL66ZT', tty(s): ttyUSB0
/sys/bus/usb/devices/2-1.3: Atmel Corp. EDBG CMSIS-DAP serial: 'ATML2412051800000464', tty(s): ttyACM1
/sys/bus/usb/devices/2-1.4: Atmel Corp. EDBG CMSIS-DAP serial: 'ATML2412051800000526', tty(s): ttyACM0
/sys/bus/usb/devices/8-1.1: FTDI FT232R USB UART serial: 'AL02AFCZ', tty(s): ttyUSB1

then eg do:
SERIAL='ATML2412051800000464' make flash
to flash via ttyACM1


