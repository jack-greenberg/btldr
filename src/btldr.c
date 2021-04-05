/*
 * @file btldr.c
 * @brief Main CAN Bootloader
 */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <inttypes.h>

#include "can_isp.h"
#include "can_lib.h"

/*
 * MCUSR := MCU Status Register
 */

void (*start_app)(void) = 0x0000;

int main(void) {
    // First, disable interrupts
    cli();

    uint8_t rst = MCUSR;
    MCUSR = 0;

    /*
     * MCUSR will be unequal to 0 if any of the following have occured:
     *   - Watchdog system reset
     *   - Brown-out reset
     *   - External reset (hardware reset on the ~RESET pin)
     *   - Power-on reset
     *
     * In all of these cases, we'll just want to jump to the application
     */
    if (rst != 0) {
        MCUSR = rst;
        start_app();
    }

    can_init();
    while (can_isp_task())
        ;

    // TODO: Verification? But if JUMP is a CAN command?
}
