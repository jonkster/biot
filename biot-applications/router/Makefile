# name of your application
APPLICATION = BIOT-Router

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-zllk

CFLAGS += -DMAX_NODES=20

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../RIOT

## add current directory to the include path. Putting it in CFLAGS will make
## it go to the beginning, before the standard includes.
CFLAGS += -I$(CURDIR)

# adding biot specific modules
DIRS += ../modules/dataCache/
USEMODULE += dataCache

#CFLAGS += -DGNRC_PKTBUF_SIZE=512 -DGNRC_IPV6_NETIF_ADDR_NUMOF=4 -DGNRC_IPV6_NC_SIZE=1
LINKFLAGS += -Wl,--no-wchar-size-warning 

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
