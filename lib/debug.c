#include "debug.h"

#include <avr/io.h>

void log_init(void) {
    LINCR |= _BV(LENA) | 0x7;
    LINBRR = 0x0C;
}

void log_uart(char* msg) {
    while (*msg) {
        loop_until_bit_is_clear(LINSIR, LBUSY);
        LINDAT = (uint8_t)*msg;
        msg++;
    }

    loop_until_bit_is_clear(LINSIR, LBUSY);
    LINDAT = (uint8_t)'\n';

    loop_until_bit_is_clear(LINSIR, LBUSY);
    LINDAT = (uint8_t)'\r';
}
