/*
 * @file main.c
 * @brief Main program to be run
 *
 * @author Jack Greenberg <j@jackgreenberg.co>
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "config.h"

static void SystemInit(void) {
    sei();
}

int main(void) {

    SystemInit();
    DDRB |= _BV(PB0);

    for (;;) {
        PORTB ^= _BV(PB0);
        _delay_ms(1000);
    }
}

