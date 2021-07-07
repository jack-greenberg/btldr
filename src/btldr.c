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
#include "image.h"  // todo: remove

/*
 * MCUSR := MCU Status Register
 */

int main(void) {
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
     */
    uint8_t rst = MCUSR;
    MCUSR = 0;
    if (rst != 0) {
        MCUSR = rst;
        start_app();

        while (1);
    }

    /*
     * TODO: Clear bootflag
     */
    can_init();
    while (can_isp_task() == CAN_ISP_ST_OK);

    while (1);  // TODO: How should we indicate that we got here?
}
