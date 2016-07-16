APPLICATION = imu_mpu9150

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

USEMODULE += mpu9150
USEMODULE += xtimer
USEMODULE += printf_float


# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../../RIOT

## add current directory to the include path. Putting it in CFLAGS will make
## it go to the beginning, before the standard includes.
CFLAGS += -I$(CURDIR)

LINKFLAGS += -Wl,--no-wchar-size-warning 

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
