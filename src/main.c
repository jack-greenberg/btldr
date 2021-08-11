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

#define LED0 (PD6)
#define BLINK_DELAY (250)  // ms

image_hdr_t image_hdr __attribute__((section(".image_hdr"))) = {
    .image_magic = IMAGE_MAGIC,
    .git_sha = GIT_SHA,
};

int main(void) {
    sei();
    DDRD |= _BV(LED0);

    log_init();
    log_uart("-- App --");

    for (;;) {
        PORTD ^= _BV(LED0);
        _delay_ms(BLINK_DELAY);
    }
}
