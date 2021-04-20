#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "can_isp.h"
#include "can_lib.h"

void (*start_app)(void) = 0x0000;

bool is_programming = false;

CAN_isp_status can_isp_task(void) {
    CAN_isp_status st = CAN_ISP_ST_SUCCESS;
    CAN_status can_st = CAN_ST_OK;

    uint8_t data[CAN_MAX_MSG_LENGTH];

    Can_msg msg = {
        .data=data,
        // Listen for any CAN message
        .mask=CAN_ISP_MASK,
        .id=0x00,
        // Accept up to 8 bytes
        .length=CAN_MAX_MSG_LENGTH,
    };

    can_st = can_receive(&msg);
    if (can_st != CAN_ST_OK) {
        // transmit error?
    }

    size_t timeout_count = 0;
    while(can_poll_complete(&msg) == CAN_ST_NOT_READY) {
        timeout_count += 1;
        if (timeout_count >= CAN_POLL_TIMEOUT) {
            // error
        }
    }

    if (msg.id != CAN_ID_PROG_START && !is_programming) {
        start_app();
    }

    switch (msg.id) {

        case CAN_ID_NODE_SELECT: {
            can_node_select(msg.data);
            break;
        }

        case CAN_ID_PROG_START: {
            can_prog_start(msg.data);
            break;
        }

        case CAN_ID_DATA: {
            can_data(msg.data, msg.length);
            break;
        }

        case CAN_ID_READ:
            break;

        case CAN_ID_START_APP:
            break;

        case CAN_ID_VALIDATE:
            break;

        case CAN_ID_VERSION:
            break;

        default:
            break;
    }

    return st;
}
