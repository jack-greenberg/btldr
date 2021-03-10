# Author: Jack Greenberg <j@jackgreenberg.co>
PROJECT = btldr
MCU = atmega16m1
F_CPU = 16000000UL
PROGRAMMER = usbasp

APP = main
BOOTLOADER = btldr

# Structure
SRC_DIR = src
BUILD_DIR = build
# SRCS = $(SRC_DIR)/*.c
# BOOT_SRCS = $(SRC_DIR)/btldr.c

# Libraries
LIB_DIR = lib
LIBS = (
	can
)

# Toolchain
PREFIX = avr-
CC      = $(PREFIX)gcc
LD      = $(PREFIX)ld
AR      = $(PREFIX)ar
OBJCOPY = $(PREFIX)objcopy
AVRDUDE = avrdude

# Flags
CFLAGS = -mmcu=$(MCU) -g -Os -Wall -Wunused -Werror
LDFLAGS = -mmcu=$(MCU) -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map --gc-sections -ffunction-sections -fdata-sections
AVRDUDE_FLAGS = -p $(MCU) -c $(PROGRAMMER) -P usb -v

# Fuses
# Calculate here: https://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega16m1
LFUSE = lfuse:w:0x65:m
HFUSE = hfuse:w:0x00:m
EFUSE = efuse:w:0xFF:m
LOCKBITS_LOCK = lock:w:0x0F:m
LOCKBITS_UNLOCK = lock:w:0x3F:m

.PHONY: clean fuses app_flash btldr_flash bootloader_size

all: $(BUILD_DIR)/$(APP).hex $(BUILD_DIR)/$(BOOTLOADER).hex
	@echo To flash bootloader run `make btldr_flash`
	@echo To flash app run `make app_flash`

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c  $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.elf: $(BUILD_DIR)/%.o
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

fuses:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U $(LFUSE) -U $(HFUSE) -U $(EFUSE)

app_flash: $(BUILD_DIR)/$(APP).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$<:i

btldr_flash: $(BUILD_DIR)/$(BOOTLOADER).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U $(LOCKBITS_UNLOCK) -U flash:w:$<:i -U $(LOCKBITS_LOCK)

bootloader_size: $(BUILD_DIR)/$(BOOTLOADER).elf
	$(AVRSIZE) --format=avr --mcu=$(MCU) $<

clean:
	rm -r $(BUILD_DIR)/*
