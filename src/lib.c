#include "lib.h"

#include <string.h>

#include "can_isp.h"
#include "can_lib.h"
#include "image.h"
#include "shared_mem.h"

static Can_msg_t can_msg;
static uint16_t fixed_ecu_id;

void updater_init(uint16_t ecu_id, uint8_t mob) {
    fixed_ecu_id = ecu_id;

    can_msg.id = (fixed_ecu_id << 4);
    can_msg.mask = 0x7f0;  // Filter in requests to this ECU
    can_msg.mob = mob;

    can_receive(&can_msg);
}

static void do_reset(uint8_t* data, uint8_t dlc) {
    if (data[0] == RESET_REQUEST_UPDATE) {
        bootflag_set(UPDATE_REQUESTED);
    }

    asm("jmp 0x3000");
}

static void do_query(uint8_t* data, uint8_t dlc) {
    // Return bootloader version from EEPROM
    uint8_t version = updater_get_version();
    uint8_t chip = CHIP_AVR_ATMEGA16M1;

    // Current timestamp from data
    uint64_t timestamp = (uint64_t)*data;
    uint64_t flash_timestamp = get_image_timestamp();
    uint64_t delta = timestamp - flash_timestamp;
    uint32_t delta_32 = (uint32_t)delta & 0xFFFF;

    uint8_t resp_data[8] = {version, chip, 0};

    memcpy((resp_data + 4), &delta_32, sizeof(delta_32));

    can_msg.id = CAN_ID_QUERY_RESPONSE;
    can_msg.data = resp_data;
    can_msg.length = 8;

    (void)can_transmit(&can_msg);
}

void updater_loop(void) {
    CAN_status st = can_poll_complete(&can_msg);

    if ((st == CAN_ST_NOT_READY) || (st == CAN_ST_ERROR)) {
        return;
    }

    switch (can_msg.id) {
        case CAN_ID_QUERY: {
            do_query(can_msg.data, can_msg.length);
        } break;
        case CAN_ID_RESET: {
            do_reset(can_msg.data, can_msg.length);
        } break;
        default: {
            // Ignore all other messages
        } break;
    }

    can_msg.id = (fixed_ecu_id << 4);
    can_msg.mask = 0x7f0;  // Filter in requests to this ECU
    can_receive(&can_msg);
}
