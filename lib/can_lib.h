/*
 * Copyright 2021 Jack Greenberg <j@jackgreenberg.co>
 */

#pragma once
#include <stdint.h>

#define CAN_POLL_TIMEOUT (100)

typedef enum {
    CAN_ST_OK = 0x00,
    CAN_ST_ERROR = 0x01,
    CAN_ST_NOT_READY,
    CAN_ST_TIMEOUT,
} CAN_status;

typedef struct {
    uint8_t *data;
    uint8_t mob;
    uint16_t mask;
    uint16_t id;
    uint8_t length;
} Can_msg;

/*
 * Extern call to can_drv.h
 */
void can_init(void);

/*
 * Initializes CAN message reception
 *
 * returns:
 *   CAN_ST_OK    - Message set for reception
 *   CAN_ST_ERROR - No free MObs
 */
CAN_status can_receive(Can_msg *msg);

/*
 * Sets CAN message to be sent
 *
 * returns:
 *   CAN_ST_OK    - Message set for transmission
 *   CAN_ST_ERROR - No free MObs
 */
CAN_status can_transmit(Can_msg *msg);

/*
 * Checks for completion of CAN command
 *
 * returns:
 *   CAN_ST_OK        - Message sent/received
 *   CAN_ST_ERROR     - Message error
 *   CAN_ST_NOT_READY - Message not sent/received
 *   CAN_ST_TIMEOUT   - Message timeout
 */
CAN_status can_poll_complete(Can_msg *msg);
