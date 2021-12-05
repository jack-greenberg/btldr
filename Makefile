# Author: Jack Greenberg <j@jackgreenberg.co>
PROJECT = btldr
MCU = atmega16m1
F_CPU = 4000000
PROGRAMMER = usbasp
BOOTSTART = 0x3000

# Structure
TARGET = btldr
APP = main
CLIENT = updatr

BUILD_DIR = build

SRCS_APP = \
		   src/main.c \
		   src/lib.c \
		   lib/can_lib.c \
		   lib/image.c \
		   lib/shmem.c \
		   src/commands.c \
		   lib/flash.c \
		   src/can_isp.c

SRCS_BOOT = \
			src/btldr.c \
			src/can_isp.c \
			src/commands.c

SRCS_SHMEM = \
			 lib/shmem.c

SRCS_CLIENT = \
			  client/main.c \
			  client/can.c \
			  client/ping.c \
			  client/flash.c \
			  third_party/log/src/log.c

SRCS_SHARED = \
			  lib/image.c \
			  lib/can_lib.c \
			  lib/flash.c \
			  lib/debug.c \
			  lib/crc32.c \
			  $(SRCS_SHMEM)

INCLUDES += lib src

SRCS_APP += lib/crc32.c lib/debug.c
SRCS_BOOT += $(SRCS_SHARED)

LDSCRIPT=atmega16m1.ld

# Toolchain
PREFIX = avr-
CC      = $(PREFIX)gcc
LD      = $(PREFIX)ld
AR      = $(PREFIX)ar
OBJCOPY = $(PREFIX)objcopy
SIZE    = $(PREFIX)size
AVRDUDE = avrdude

# Flags
CFLAGS = \
		 -mmcu=$(MCU) \
		 -Os \
		 -g \
		 -Wall \
	   	 -Wunused \
		 -fno-jump-tables \
		 -ffunction-sections \
		 -Werror

CFLAGS_CLIENT = \
				-Wall \
				-Werror \
				-Ithird_party/log/src \
				-Isrc \
				-Ilib \
				-DLOG_USE_COLOR \
				-O2

LDFLAGS = \
		  -mmcu=$(MCU) \
		  -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map \
		  -static \
		  -nostartfiles \
		  -Wl,-u,vfprintf -lprintf_min \
		  -T $(LDSCRIPT)

EEPFLAGS = \
		   -j .eeprom \
		   --change-section-lma .eeprom=0 \
		   --set-section-flags=.eeprom="alloc,load"

AVRDUDE_FLAGS = \
				-p 16m1 \
				-c $(PROGRAMMER) \
				-P usb \
				-v

GIT_SHA := \"$(shell git rev-parse --short HEAD)\"

DEFINES += GIT_SHA=$(GIT_SHA) F_CPU=$(F_CPU) BTLDR_ID=0x01
CFLAGS += $(foreach d,$(DEFINES),-D$(d))
CFLAGS += $(foreach i,$(INCLUDES),-I$(i))

# Fuses
# Calculate here: https://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega16m1&LOW=65&HIGH=D8&EXTENDED=FE&LOCKBIT=CF
LFUSE = lfuse:w:0x65:m
HFUSE = hfuse:w:0xD8:m
EFUSE = efuse:w:0xFE:m
LOCKBITS_LOCK = lock:w:0xCF:m
LOCKBITS_UNLOCK = lock:w:0xFF:m

.PHONY: all
all: $(BUILD_DIR)/$(PROJECT).hex $(BUILD_DIR)/$(APP).bin $(BUILD_DIR)/$(PROJECT).eep

$(BUILD_DIR)/%.eep: $(BUILD_DIR)/%.elf
	$(OBJCOPY) $(EEPFLAGS) -O ihex $< $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf client
	$(OBJCOPY) $< $@ -R .eeprom -O ihex

.PHONY: client
client: $(BUILD_DIR)/$(CLIENT)

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf patch_header
	$(OBJCOPY) -O binary -R .eeprom $< $@
	$(BUILD_DIR)/patch_header $@

$(BUILD_DIR)/$(PROJECT).elf: $(SRCS_BOOT)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,--section-start=.text=$(BOOTSTART) $^ -o $@

$(BUILD_DIR)/$(APP).elf: $(SRCS_APP)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

patch_header: scripts/patch_header.c
	@mkdir -p $(BUILD_DIR)
	gcc -DDEBUG -Ilib -Wall -o $(BUILD_DIR)/$@ $^ lib/crc32.c

flash: $(BUILD_DIR)/$(APP).bin $(BUILD_DIR)/$(PROJECT).hex $(BUILD_DIR)/$(PROJECT).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U $(LFUSE) \
		-U $(HFUSE) \
		-U $(EFUSE) \
		-U $(LOCKBITS_UNLOCK) \
		-U eeprom:w:$(BUILD_DIR)/$(PROJECT).eep:i \
		-U flash:w:$(BUILD_DIR)/$(PROJECT).hex:i \
		-U $(LOCKBITS_LOCK) \
		-U flash:w:$(BUILD_DIR)/$(APP).bin:r

$(BUILD_DIR)/$(CLIENT): $(SRCS_CLIENT)
	@mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS_CLIENT) $^ -o $@

%.size: %
	nm --size-sort --print-size -td $< | sort >> $@
	echo "" >> $@
	stat $< --printf "%s bytes\n" > $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)/*

.PHONY: lint
lint:
	clang-format -i --style=file lib/* src/* scripts/* client/*.c client/*.h

docs/state.png: docs/state.puml
	plantuml $<
