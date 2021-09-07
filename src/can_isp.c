#include "can_isp.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <util/delay.h>

#include "can_lib.h"
#include "debug.h"
#include "image.h"

uint8_t can_isp_task(void) {
    uint8_t st = 0;

    uint8_t data[CAN_MAX_MSG_LENGTH];
    Can_msg_t msg = {
        .data = data,
        .mask = CAN_ISP_MASK,
        .id = 0x00,
        .length = CAN_MAX_MSG_LENGTH,
    };

    // Receive CAN message. This shouldn't error because we always restore our
    // message objects
    (void)can_receive(&msg);

    while (can_poll_complete(&msg) == CAN_ST_NOT_READY)
        ;

    switch (msg.id) {
        case CAN_ID_QUERY: {
            st = handle_query(msg.data, msg.length);
            break;
        }
        case CAN_ID_RESET: {
            st = handle_reset(msg.data, msg.length);
            if (st != 0) {
                // Image is invalid
            }
            break;
        }
        case CAN_ID_REQUEST: {
            st = handle_request(msg.data, msg.length);
            break;
        }
        case CAN_ID_DATA: {
            st = handle_data(msg.data, msg.length);
            break;
        }
        default: {
            uint8_t data[1] = {
                ERR_INVALID_COMMAND,
            };

            Can_msg_t msg = {
                .mob = CAN_AUTO_MOB,
                .mask = CAN_NO_FILTERING,
                .id = CAN_ID_STATUS,
                .data = data,
                .length = 4,
            };
            st = can_transmit(&msg);
            break;
        }
    }

    return st;
}
