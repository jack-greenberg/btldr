/*
 * @file main.c
 * @brief Main program to be run
 *
 * @author Jack Greenberg <j@jackgreenberg.co>
 */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "image.h"
#include "config.h"

image_hdr_t image_hdr __attribute__((section(".image_hdr"))) = {
    .image_magic = IMAGE_MAGIC,
    .git_sha = GIT_SHA,
};

int main(void) {
    sei();
    DDRB |= _BV(PB0);

    for (;;) {
        PORTB ^= _BV(PB0);
        _delay_ms(1000);
    }
}
