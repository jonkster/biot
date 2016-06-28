*This is a very experimental setup.  Code is ugly.*

# Overview

Rough code demoing OLED display on ATSAMR21ZLL-EK board

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


