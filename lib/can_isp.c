#include "can_isp.h"
#include "can_lib.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static bool is_programming = false;

// Current (unprogrammed) memory address
static uint16_t program_memory_address;

// Number of bytes remaining to program
static uint16_t program_memory_num_bytes;

// static CAN_status can_isp_respond(Can_msg* msg) {
// 
// }

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

    switch (msg.id) {

        case CAN_ID_NODE_SELECT: {
            if (msg.data[0] == NODE_ID) {
                is_programming = true;
            } else {
                // Jump to application? Stall?
            }
            break;
        }

        case CAN_ID_PROG_START: {
            uint16_t start_addr = (msg.data[1] << 8) | msg.data[2];
            uint16_t end_addr = (msg.data[3] << 8) | msg.data[4];
            program_memory_address = start_addr;
            program_memory_num_bytes = (end_addr - start_addr) + 1;
            break;
        }

        case CAN_ID_DATA:
            break;

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
