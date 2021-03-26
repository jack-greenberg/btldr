/*
 * @file main.c
 * @brief Main program to be run
 *
 * @author Jack Greenberg <j@jackgreenberg.co>
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"

static void SystemInit(void) {
    sei();
}

int main(void) {

    SystemInit();

    for (;;) {

    }
}

