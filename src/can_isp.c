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
    can_frame_t msg = {
        .data = data,
        .id = 0x00,
        .dlc = CAN_MAX_MSG_LENGTH,
    };

    can_filter_t filter = {
        .mask = 0x00,
        .id = 0x00,
    };

    // Receive CAN message. This shouldn't error because we always restore our
    // message objects
    (void)can_receive(&msg, filter);

    do {
    } while ((st = can_poll_receive(&msg)) == -1);

    if (st == 1) {
        log_uart("Error!");
        return 0;
    }

    switch (msg.id) {
        case CAN_ID_QUERY: {
            st = handle_query(msg.data, msg.dlc);
            break;
        }
        case CAN_ID_RESET: {
            st = handle_reset(msg.data, msg.dlc);
            if (st != 0) {
                // Image is invalid
            }
            break;
        }
        case CAN_ID_REQUEST: {
            st = handle_request(msg.data, msg.dlc);
            break;
        }
        case CAN_ID_DATA: {
            st = handle_data(msg.data, msg.dlc);
            break;
        }
        default: {
            uint8_t data[1] = {
                ERR_INVALID_COMMAND,
            };

            can_frame_t msg = {
                .mob = 0,  // TODO
                .id = CAN_ID_STATUS,
                .data = data,
                .dlc = 4,
            };
            st = can_send(&msg);
            break;
        }
    }

    log_uart("End of loop");

    return st;
}
