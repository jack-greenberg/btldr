# Author: Jack Greenberg <j@jackgreenberg.co>
PROJECT = btldr-example
MCU = atmega16m1
F_CPU = 16000000UL
PROGRAMMER = usbasp
BOOTSTART = 0x3000

# Structure
TARGET = btldr
BUILD_DIR = build

SRCS_BOOT = \
			src/btldr.c \
			# src/image.c

SRCS_SHARED = \
	lib/can_drv.c \
	lib/can_lib.c \
	lib/can_isp.c \
	lib/can_isp_commands.c \
	lib/flash.c \
	lib/crc32.c

INCLUDES += lib

SRCS_BOOT += $(SRCS_SHARED)

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
		 -g \
		 -Os \
		 -Wall \
	   	 -Wunused \
		 -fno-jump-tables \
		 -Werror

LDFLAGS = \
		  -mmcu=$(MCU) \
		  -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map \
		  -static \
		  -nostartfiles \
		  -Wl,--build-id \
		  -Wl,--section-start=.text=$(BOOTSTART)

AVRDUDE_FLAGS = \
				-p $(MCU) \
				-c $(PROGRAMMER) \
				-P usb -v

GIT_SHA := \"$(shell git rev-parse --short HEAD)\"

DEFINES += GIT_SHA=$(GIT_SHA)
CFLAGS += $(foreach d,$(DEFINES),-D$(d))
CFLAGS += $(foreach i,$(INCLUDES),-I$(i))

# Fuses
# Calculate here: https://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega16m1
LFUSE = lfuse:w:0x65:m
HFUSE = hfuse:w:0x00:m
EFUSE = efuse:w:0xFF:m
LOCKBITS_LOCK = lock:w:0x0F:m
LOCKBITS_UNLOCK = lock:w:0x3F:m

.PHONY: all

all: $(BUILD_DIR)/$(PROJECT).bin size

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) $^ $@ -O binary

$(BUILD_DIR)/$(PROJECT).elf: $(SRCS_BOOT)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# fuses:
# 	$(AVRDUDE) $(AVRDUDE_FLAGS) -U $(LFUSE) -U $(HFUSE) -U $(EFUSE)

# app_flash: $(BUILD_DIR)/$(APP).hex
# 	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$<:i

.PHONY: size
size: $(BUILD_DIR)/$(PROJECT).elf
	$(SIZE) --format=avr --mcu=$(MCU) $<

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)/*

.PHONY: lint
lint:
	clang-format -i --style="{BasedOnStyle: google, IndentWidth: 4}" lib/* src/*
