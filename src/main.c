/*
 * @file main.c
 * @brief Main program to be run
 *
 * @author Jack Greenberg <j@jackgreenberg.co>
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "type.h"

void main(void) {

    SystemInit();

    for (;;) {

    }
}

void SystemInit(void) {
    sei();
}
