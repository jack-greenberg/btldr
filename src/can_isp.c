#include "can_isp.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "can_lib.h"
#include "debug.h"
#include "image.h"

int start_app(void) {
    const image_hdr_t* hdr = image_get_header();
    int valid = image_validate(hdr);

    if (valid == IMAGE_VALID) {
        asm("jmp %0" ::"I"(sizeof(image_hdr_t)));
    } else {
        return valid;
    }
    return 0;
}

bool is_programming = false;

CAN_isp_status can_isp_task(void) {
    CAN_isp_status st = CAN_ISP_ST_OK;

    uint8_t data[CAN_MAX_MSG_LENGTH];

    Can_msg msg = {
        .data = data,
        // Listen for any CAN message
        .mask = CAN_ISP_MASK,
        .id = 0x00,
        // Accept up to 8 bytes
        .length = CAN_MAX_MSG_LENGTH,
    };

    // Receive CAN message
    can_receive(&msg);
    // This shouldn't error because we always restore our MObs

    // TODO: what to do if there's an error
    // TODO: Do we need a timeout?
    while (can_poll_complete(&msg) == CAN_ST_NOT_READY)
        ;

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
        case CAN_ID_START_APP: {
            can_start_app(msg.data, msg.length);
            break;
        }
        default: {
            // TODO: Transmit error
            break;
        }
    }

    return st;
}
