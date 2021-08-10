/*
 * Copyright 2021 Jack Greenberg <j@jackgreenberg.co>
 */

#pragma once
#include <stdint.h>

#define CAN_AUTO_MOB (0xFF)
#define CAN_NO_FILTERING (0x000)

typedef enum {
    CAN_ST_OK = 0x00,
    CAN_ST_ERROR = 0x01,
    CAN_ST_NOT_READY = 0x02,
} CAN_status;

typedef struct {
    uint8_t *data;
    uint8_t mob;
    uint16_t mask;
    uint16_t id;
    uint8_t length;
} Can_msg_t;

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
CAN_status can_receive(Can_msg_t *msg);

/*
 * Sets CAN message to be sent
 *
 * returns:
 *   CAN_ST_OK    - Message set for transmission
 *   CAN_ST_ERROR - No free MObs
 */
CAN_status can_transmit(Can_msg_t *msg);

/*
 * Checks for completion of CAN command
 *
 * returns:
 *   CAN_ST_OK        - Message sent/received
 *   CAN_ST_ERROR     - Message error
 *   CAN_ST_NOT_READY - Message not sent/received
 */
CAN_status can_poll_complete(Can_msg_t *msg);
