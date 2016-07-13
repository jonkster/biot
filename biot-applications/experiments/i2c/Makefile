# name of your application
APPLICATION = BIOT-i2c

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

ifeq ($(BOARD),samr21-xpro)
    CFLAGS += -DNOOLED=true
endif

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../../RIOT

## add current directory to the include path. Putting it in CFLAGS will make
## it go to the beginning, before the standard includes.
CFLAGS += -I$(CURDIR)

USEMODULE += printf_float
USEMODULE += posix
USEMODULE += xtimer

# Use minimal standard random number generator
USEMODULE += prng_minstd


# Add the shell and some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

LINKFLAGS += -Wl,--no-wchar-size-warning 

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
