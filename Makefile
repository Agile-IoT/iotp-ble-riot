APPLICATION = eid_ble
RIOT_DIR = $(CURDIR)/RIOT
#BOARD ?= native
BOARD ?= nrf52dk
RIOTBASE ?= $(RIOT_DIR)
QUIET ?= 1

CFLAGS += -DDEVELHELP
CFLAGS += -I$(RIOT_DIR)/sys
CFLAGS += -DCRYPTO_AES
CFLAGS += -Wno-unused-variable

FEATURES_REQUIRED += periph_flashpage
#FEATURES_OPTIONAL += periph_flashpage_raw

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEPKG += nimble


USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include

.PHONY: clean

clean:
	$(RM)
