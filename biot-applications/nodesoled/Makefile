# name of your application
APPLICATION = net_oled

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-zllk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../RIOT


## add current directory to the include path. Putting it in CFLAGS will make
## it go to the beginning, before the standard includes.
CFLAGS += -I$(CURDIR)

# adding biot specific modules
DIRS += ../modules/ssd1306/
#CFLAGS += -I../modules/ssd1306
USEMODULE += ssd1306
DIRS += ../modules/biotUdp/
USEMODULE += biotUdp

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif

# Specify the minimum networking modules for IPv6
USEMODULE += gnrc_ipv6
#USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_ndp



# Add a routing protocol
#USEMODULE += gnrc_rpl
#USEMODULE += auto_init_gnrc_rpl

# This application dumps received packets to STDIO using the pktdump module
#USEMODULE += gnrc_pktdump

# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo

# Use minimal standard PRNG
USEMODULE += prng_minstd

USEMODULE += gnrc_udp
USEMODULE += gnrc_conn_udp
# needed by biotUdp
USEMODULE += posix
USEMODULE += posix_sockets

# Add the shell and some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6

CFLAGS += -DGNRC_PKTBUF_SIZE=512 -DGNRC_IPV6_NETIF_ADDR_NUMOF=4 -DGNRC_IPV6_NC_SIZE=1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
