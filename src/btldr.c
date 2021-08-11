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
    // bool update_requested = bootflag_get(UPDATE_REQUESTED);

    if (!update_requested) {
        jump_to_app();
        log_uart("Jump failed, entering loop");

        while (1) continue;
    }

    log_uart("Update requested, going into updater");

    // Updater
    can_init();
    while (1) {
        // TODO: Log errors here?
        (void)can_isp_task();
    }
}
