/*
 * @file btldr.c
 * @brief Main CAN Bootloader
 */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "can_isp.h"
#include "can_lib.h"
#include "config.h"
#include "debug.h"
#include "image.h"

/*
 * MCUSR := MCU Status Register
 */

int main(void) {
    log_init();

    log_uart("-- Bootloader --");

    // First, disable interrupts
    cli();

    /*
     * MCUSR will be unequal to 0 if any of the following have occured:
     *   - Watchdog system reset
     *   - Brown-out reset
     *   - External reset (hardware reset on the ~RESET pin)
     *   - Power-on reset
     *
     * In all of these cases, we'll just want to jump to the application
     *
     * TODO
     *   - This might not be the best way of checking for reset conditions
     *   - A better way might be to have a section of shared memory that has a
     *   "DFU-requested" flag to check and see if we should continue into the
     *   updater.
     */
    uint8_t rst = MCUSR;
    MCUSR = 0;
    if (rst != 0) {
        MCUSR = rst;
        jump_to_app();

        while (1)
            ;
    }

    // Updater
    can_init();
    while (1) {
        // TODO: Log errors here?
        (void)can_isp_task();
    }
}
