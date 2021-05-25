# Author: Jack Greenberg <j@jackgreenberg.co>
PROJECT = btldr-example
MCU = atmega16m1
F_CPU = 4000000UL
PROGRAMMER = usbasp
BOOTSTART = 0x3000

# Structure
TARGET = btldr
APP = main
BUILD_DIR = build

SRCS_APP = \
		   src/main.c

SRCS_BOOT = \
			src/btldr.c \
			src/can_isp.c \
			src/can_isp_commands.c

SRCS_SHARED = \
			  lib/image.c \
			  lib/can_drv.c \
			  lib/can_lib.c \
			  lib/flash.c \
			  lib/crc32.c

INCLUDES += lib

SRCS_APP += lib/crc32.c
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
		 -Wall \
	   	 -Wunused \
		 -fno-jump-tables \
		 -Werror

LDFLAGS = \
		  -mmcu=$(MCU) \
		  -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map \
		  -static \
		  -nostartfiles \
		  -ffunction-sections \
		  -Wl,--build-id \
		  -T $(LDSCRIPT)

AVRDUDE_FLAGS = \
				-p m16 \
				-c $(PROGRAMMER) \
				-P usb \
				-v \
				-F

GIT_SHA := \"$(shell git rev-parse --short HEAD)\"

DEFINES += GIT_SHA=$(GIT_SHA) F_CPU=$(F_CPU)
CFLAGS += $(foreach d,$(DEFINES),-D$(d))
CFLAGS += $(foreach i,$(INCLUDES),-I$(i))

# Fuses
# Calculate here: https://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega16m1&LOW=65&HIGH=D8&EXTENDED=FF&LOCKBIT=3F
LFUSE = lfuse:w:0x65:m
HFUSE = hfuse:w:0xD8:m
EFUSE = efuse:w:0xFF:m
LOCKBITS_LOCK = lock:w:0xCF:m
LOCKBITS_UNLOCK = lock:w:0xFF:m

.PHONY: all
all: $(BUILD_DIR)/$(PROJECT).hex $(BUILD_DIR)/$(APP).hex

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf patch_header
	$(OBJCOPY) $< $@ -O ihex
	# TODO: Patch header

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) $^ $@ -O binary

$(BUILD_DIR)/$(PROJECT).elf: $(SRCS_BOOT)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,--section-start=.text=$(BOOTSTART) $^ -o $@

$(BUILD_DIR)/$(APP).elf: $(SRCS_APP)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

patch_header: scripts/patch_header.c
	@mkdir -p $(BUILD_DIR)
	gcc -DDEBUG -o $(BUILD_DIR)/$@ $^

flash: $(BUILD_DIR)/$(APP).hex $(BUILD_DIR)/$(PROJECT).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U $(LFUSE) \
		-U $(HFUSE) \
		-U $(LOCKBITS_UNLOCK) \
		-U flash:w:$(BUILD_DIR)/$(PROJECT).hex:i \
		-U $(LOCKBITS_LOCK) \
		-U flash:w:$(BUILD_DIR)/$(APP).hex:i


%.size: %
	nm --size-sort --print-size -td $< | sort >> $@
	echo "" >> $@
	stat $< --printf "%s bytes\n" > $@ # TODO: We should stat the bin file

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)/*

.PHONY: lint
lint:
	clang-format -i --style=file lib/* src/* scripts/*
