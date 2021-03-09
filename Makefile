# Author: Jack Greenberg <j@jackgreenberg.co>
PROJECT = btldr
MCU = atmega16m1
PART = m16
F_CPU = 16000000UL
PROGRAMMER = usbasp

# Structure
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
SRCS = $(SRC_DIR)/main.c
BOOT_SRCS = $(SRC_DIR)/btldr.c
# OBJECTS = $(addprefix $(OBJDIR)/, $(CSOURCES:.c=.o))

# Toolchain
PREFIX = avr-
CC      = $(PREFIX)cc
LD      = $(PREFIX)ld
AR      = $(PREFIX)ar
OBJCOPY = $(PREFIX)objcopy
AVRDUDE = avrdude

# Flags
CFLAGS = -mmcu=$(MCU) -g -0s -Wall -Wunused -Werror
LDFLAGS = -mmcu=$(MCU) -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map --gc-sections -ffunction-sections -fdata-sections
AVRDUDE_FLAGS = -p $(PART) -c $(PROGRAMMER) -P usb -v

# Fuses
# Calculate here: https://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega16m1
LFUSE = lfuse:w:0x65:m
HFUSE = hfuse:w:0x00:m
EFUSE = efuse:w:0xFF:m
LOCKBITS_LOCK = lock:w:0x0F:m
LOCKBITS_UNLOCK = lock:w:0x3F:m

.PHONY: clean fuses

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c  $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.elf: %(BUILD_DIR)/%.o
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(BUILD_DIR)/%.elf: 
	$(CC)

fuses:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U $(LFUSE) -U $(HFUSE) -U $(EFUSE)

app_flash: main.hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$<:i

btldr_flash: btldr.hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U $(LOCKBITS_UNLOCK) -U flash:w:$<:i -U $(LOCKBITS_LOCK)

.size: $(BUILDTARGET).elf
	$(AVRSIZE) --format=avr --mcu=$(MCU) $<

clean:
	@echo "Cleaning..."
	@rm -r $(BUILD_DIR)/*
	@echo "Done cleaning."

