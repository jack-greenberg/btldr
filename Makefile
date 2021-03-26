# Author: Jack Greenberg <j@jackgreenberg.co>
PROJECT = btldr-example
MCU = atmega16m1
F_CPU = 16000000UL
PROGRAMMER = usbasp

# Structure
BUILD_DIR = build

SRCS = src/main.c

SRCS_BOOT = src/btldr.c

SRCS_SHARED = \
	lib/can.c \
	lib/can_isp.c

INCLUDES += lib

SRCS_APP += $(SRCS_SHARED)
SRCS_BOOT += $(SRCS_SHARED)

# Toolchain
PREFIX = avr-
CC      = $(PREFIX)gcc
LD      = $(PREFIX)ld
AR      = $(PREFIX)ar
OBJCOPY = $(PREFIX)objcopy
AVRDUDE = avrdude

# Flags
CFLAGS = -mmcu=$(MCU) -g -Os -Wall -Wunused -Werror
LDFLAGS = -mmcu=$(MCU) -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map -static -nostartfiles
AVRDUDE_FLAGS = -p $(MCU) -c $(PROGRAMMER) -P usb -v

CFLAGS += $(foreach i,$(INCLUDES),-I$(i))

# Fuses
# Calculate here: https://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega16m1
LFUSE = lfuse:w:0x65:m
HFUSE = hfuse:w:0x00:m
EFUSE = efuse:w:0xFF:m
LOCKBITS_LOCK = lock:w:0x0F:m
LOCKBITS_UNLOCK = lock:w:0x3F:m

.PHONY: all

all: $(BUILD_DIR)/$(PROJECT).bin

# Combines the binaries
$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT)-app.bin $(BUILD_DIR)/$(PROJECT)-bootloader.bin
	@cat $^ > $@

$(BUILD_DIR)/$(PROJECT)-%.bin: $(BUILD_DIR)/$(PROJECT)-%.elf
	$(OBJCOPY) $^ $@ -O binary

$(BUILD_DIR)/$(PROJECT)-app.elf: $(SRCS_APP)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/$(PROJECT)-bootloader.elf: $(SRCS_BOOT)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# fuses:
# 	$(AVRDUDE) $(AVRDUDE_FLAGS) -U $(LFUSE) -U $(HFUSE) -U $(EFUSE)

# app_flash: $(BUILD_DIR)/$(APP).hex
# 	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$<:i

# btldr_flash: $(BUILD_DIR)/$(BOOTLOADER).hex
# 	$(AVRDUDE) $(AVRDUDE_FLAGS) -U $(LOCKBITS_UNLOCK) -U flash:w:$<:i -U $(LOCKBITS_LOCK)

# bootloader_size: $(BUILD_DIR)/$(BOOTLOADER).elf
# 	$(AVRSIZE) --format=avr --mcu=$(MCU) $<

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)/*
