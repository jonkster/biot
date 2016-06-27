# name of your application
APPLICATION = ADC

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-zllk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../RIOT

#ifeq (,$(SLIP_UART))
#  # set default (last available UART)
#  SLIP_UART="UART_DEV(UART_NUMOF-1)"
#endif
#ifeq (,$(SLIP_BAUDRATE))
#  # set default
#  SLIP_BAUDRATE=115200
#endif

#GNRC_NETIF_NUMOF := 2
INCLUDES += -I$(CURDIR)
#CFLAGS += -DSLIP_UART=$(SLIP_UART)
#CFLAGS += -DSLIP_BAUDRATE=$(SLIP_BAUDRATE)

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += xtimer
#USEMODULE += gnrc_netdev_default
#USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6
#USEMODULE += gnrc_ipv6_router_default
# Include SLIP package for IP over Serial communication
#USEMODULE += gnrc_slip
#USEMODULE += gnrc_udp
#USEMODULE += gnrc_sixlowpan_border_router_default
# Add a routing protocol
#USEMODULE += gnrc_rpl
# This application dumps received packets to STDIO using the pktdump module
#USEMODULE += gnrc_pktdump
# Add forwarding table
#USEMODULE += fib
# Additional networking modules that can be dropped if not needed
#USEMODULE += gnrc_icmpv6
#USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
#USEMODULE += posix_sockets

#USE_MODULE += periph_adc
#DISABLE_MODULE += gnrc_sixlowpan_iphc_nhc

#FEATURES_REQUIRED = periph_spi
#FEATURES_REQUIRED += periph_gpio

#USEMODULE += gnrc_conn_udp
#USEPKG += microcoap

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
