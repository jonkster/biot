# name of your application
APPLICATION = ADC

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-zllk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../../RIOT

INCLUDES += -I$(CURDIR)

USEMODULE += xtimer
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

#FEATURES_REQUIRED = periph_spi
#FEATURES_REQUIRED += periph_gpio

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP
LINKFLAGS += -Wl,--no-wchar-size-warning

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
