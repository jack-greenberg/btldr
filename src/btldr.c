/*
 * @file btldr.c
 * @brief Main CAN Bootloader
 */
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>

#include "can_isp.h"
#include "can_lib.h"
#include "debug.h"
#include "image.h"
#include "shared_mem.h"

/*
 * MCUSR := MCU Status Register
 */

extern uint32_t bootflags;

int main(void) {
    cli();  // Disable interrupts in the btldr

    log_init();
    log_uart("-- Bootloader --");

    uint32_t flags = eeprom_read_dword(&bootflags);
    bool update_requested = ((flags & UPDATE_REQUESTED) != 0);
    bool image_is_valid = ((flags & IMAGE_IS_VALID) != 0);
    // bool update_requested = bootflag_get(UPDATE_REQUESTED);

    if (!update_requested) {
        if (image_is_valid) {
            // Jump to application with offset of image header size
            asm("jmp %0" ::"I"(sizeof(image_hdr_t)));

            log_uart("Jump failed, entering loop");
            while (1) continue;
        } else {
            log_uart("Image is corrupted or invalid, going into updater");
        }
    } else {
        log_uart("Update requested, going into updater");
    }

    // Updater
    can_init(BAUD_500KBPS);
    while (1) { (void)can_isp_task(); }
}
