/*
 * @file btldr.c
 * @brief Main CAN Bootloader
 */
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "can.h"

/*
 * MCUSR := MCU Status Register
 */

void (*start_app)(void) = 0x0000;

int main(void)
{
    // First, disable interrupts
    // asm("cli");
    __disable_interrupt();

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
    if (MCUSR != 0) {
        start_app();
    }

    Can_init();
    Can_ISP_init();
    while(Can_ISP_task());
}
