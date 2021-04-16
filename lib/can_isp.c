#include "can_isp.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "can_lib.h"

void (*start_app)(void) = 0x0000;

static uint32_t crc = 0x00;

bool is_programming = false;

CAN_isp_status can_isp_task(void) {
    CAN_isp_status st = CAN_ISP_ST_SUCCESS;
    CAN_status can_st = CAN_ST_OK;

    uint8_t data[CAN_MAX_MSG_LENGTH];

    Can_msg msg = {
        .data = data,
        // Listen for any CAN message
        .mask = CAN_ISP_MASK,
        .id = 0x00,
        // Accept up to 8 bytes
        .length = CAN_MAX_MSG_LENGTH,
    };

    can_st = can_receive(&msg);
    if (can_st != CAN_ST_OK) {
        // transmit error?
    }

    size_t timeout_count = 0;
    while (can_poll_complete(&msg) == CAN_ST_NOT_READY) {
        timeout_count += 1;
        if (timeout_count >= CAN_POLL_TIMEOUT) {
            // error
        }
    }

    // if (msg.id != CAN_ID_SESSION_START && !is_programming) {
    //     // TODO What does the ECU do if it's not being updated?
    //     start_app();
    // }

    switch (msg.id) {
        case CAN_ID_NODE_SELECT: {
            st = can_node_select(msg.data, msg.length);
            break;
        }

        case CAN_ID_SESSION_START: {
            can_session_start(msg.data, msg.length);
            break;
        }

        case CAN_ID_DATA: {
            can_data(msg.data, msg.length);
            break;
        }

        case CAN_ID_START_APP:
            can_start_app(msg.data, msg.length);
            break;

        default:
            break;
    }

    return st;
}
