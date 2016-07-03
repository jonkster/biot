# name of your application
APPLICATION = BIOT

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../../RIOT

INCLUDES += -I$(CURDIR)

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6
USEMODULE += gnrc_ipv6_router_default
# Include SLIP package for IP over Serial communication
USEMODULE += gnrc_udp
# Add a routing protocol
USEMODULE += gnrc_rpl
# Add forwarding table
USEMODULE += fib
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += posix_sockets

USEMODULE += gnrc_conn_udp

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP
LINKFLAGS += -Wl,--no-wchar-size-warning

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
