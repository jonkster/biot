# name of your application
APPLICATION = BIOT-Router

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-zllk

# if a single node then define MAX_NODES to 0
CFLAGS += -DMAX_NODES=20

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../RIOT

## add current directory to the include path. Putting it in CFLAGS will make
## it go to the beginning, before the standard includes.
CFLAGS += -I$(CURDIR)

USEMODULE += printf_float

# adding biot specific modules
DIRS += ../modules/biotUdp/
USEMODULE += biotUdp
DIRS += ../modules/identify/
USEMODULE += biotIdentify
#DIRS += ../modules/sendData/
#USEMODULE += sendData
DIRS += ../modules/imu/
USEMODULE += imu
#DIRS += ../modules/dataCache/
#USEMODULE += dataCache

#
# create 2 network interfaces, one for 6lowPAN and one for SLIP
GNRC_NETIF_NUMOF := 2
ifeq (,$(SLIP_UART))
  # set default (last available UART)
  SLIP_UART="UART_DEV(UART_NUMOF-1)"
endif
ifeq (,$(SLIP_BAUDRATE))
  # set default
  SLIP_BAUDRATE=115200
endif
CFLAGS += -DSLIP_UART=$(SLIP_UART)
CFLAGS += -DSLIP_BAUDRATE=$(SLIP_BAUDRATE)

# kludge to get both samr21b18-mz210pa board and samr21-zllk board working with
# same RIOT/cpu/samd21/periph/uart.c file
CFLAGS += -DZLLK
# may be useful when debugging to allow compiling with unused variables
#CFLAGS += -Wno-unused-variable

# Include SLIP package for IP over Serial communication
USEMODULE += gnrc_slip


# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif

# Specify the minimum networking modules for IPv6
# we can have either gnrc_ipv6 or gnrc_ipv6_router_default depending on needs
USEMODULE += gnrc_ipv6
USEMODULE += gnrc_icmpv6_echo

# UDP communication stuff
#USEMODULE += gnrc_udp
USEMODULE += gnrc_conn_udp
# needed by biotUdp
USEMODULE += posix
USEMODULE += posix_sockets


# Add a routing protocol
USEMODULE += gnrc_rpl
# Add forwarding table
USEMODULE += fib


# Use minimal standard random number generator
USEMODULE += prng_minstd

# Add the shell and some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6

#CFLAGS += -DGNRC_PKTBUF_SIZE=512 -DGNRC_IPV6_NETIF_ADDR_NUMOF=4 -DGNRC_IPV6_NC_SIZE=1
LINKFLAGS += -Wl,--no-wchar-size-warning 

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

#CFLAGS += -DDEVELHELP -DNDEBUG # uncomment to allow detailed ps info

include $(RIOTBASE)/Makefile.include
