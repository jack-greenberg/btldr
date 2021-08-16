/*
 * Author: Jack Greenberg <j@jackgreenberg.co>
 * Copyright 2021
 */
#include "can_client.h"
#include <linux/can.h>
#include <linux/can/raw.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

extern bool verbose;

int cmd_flash(uint8_t ecu_id, char* binary_path) {
    if (verbose) {
        printf("Flashing id 0x%X with binary at %s\n", ecu_id, binary_path);
    }
    return 0;
}

int cmd_ping(uint8_t ecu_id) {
    if (verbose) {
        printf("Pinging id 0x%X\n", ecu_id);
    }
    return 0;
}
