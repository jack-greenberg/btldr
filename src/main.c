/*
 * @file main.c
 * @brief Main program to be run
 *
 * @author Jack Greenberg <j@jackgreenberg.co>
 */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "config.h"
#include "debug.h"
#include "image.h"

image_hdr_t image_hdr __attribute__((section(".image_hdr"))) = {
    .image_magic = IMAGE_MAGIC,
    .git_sha = GIT_SHA,
};

int main(void) {
    sei();
    DDRB |= _BV(PB1);

    log_init();
    log_uart("App initialized", 16);

    for (;;) {
        PORTB ^= _BV(PB1);
        _delay_ms(250);
    }
}
