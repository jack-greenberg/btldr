#pragma once

#include <stdint.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "image.h"
#include "log.h"

#include "can.h"
#include "can_isp.h"

#define POLLTIMEOUT (1000)
#define MAX_RETRIES (5)
#define PING_BROADCAST (0xFF)

/*
 * Pings a CAN device
 */
int cmd_ping(can_client_t* client, uint8_t ecu_id, uint8_t* current_image);

/*
 * Flashes a binary
 */
int cmd_flash(can_client_t* client, uint8_t ecu_id, FILE* fp);
