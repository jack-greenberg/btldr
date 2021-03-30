#include <avr/io.h>
#include "can.h"
#include <inttypes.h>
#include <stddef.h>

void Can_init(void) {
    // Reset Can peripheral
    CANGCON = 1 << SWRES;

    // Set baudrate to 500kbps
    CANBT1 = 0x00;
    CANBT2 = 0x04;
    CANBT3 = 0x12;

    // Clear all Message OBjects (MObs)
    size_t mob;
    for (mob = 0; mob < CAN_NUM_MOBS; mob++) {
        CANPAGE = (mob << 4);
        CANSTMOB = 0x00;
        CANCDMOB = 0x00;
    }

    // Re-enable CAN
    CANGCON = 1 << ENASTB;
}

CAN_DRV_status Can_wait_on_receive(uint16_t id, uint16_t mask, uint8_t msg_length) {
    CAN_DRV_status st = CAN_DRV_ST_OK;

    // Select CAN mob based on input MOb
    CANPAGE = (MOB_0 << MOBNB0);

    // Clean CAN status for this MOb
    CANSTMOB = 0x00;

    // Set MOb ID
    CANIDT1 = (uint8_t) (id >> 3); // node ID
    CANIDT2 = (uint8_t) (id << 5);
    CANIDT3 = 0x00;
    CANIDT4 = 0x00; // Data frame

    // Set up MASK
    CANIDM1 = (uint8_t) (mask >> 3);
    CANIDM2 = (uint8_t) (mask << 5);
    CANIDM3 = 0x00;
    CANIDM4 = (_BV(RTRMSK) | _BV(IDEMSK)); // Use what is set above

    // Begin waiting for Rx
    CANCDMOB = 0x00;
    CANCDMOB = (0x02 << CONMOB0) | (msg_length << DLC0);

    return st;
}

CAN_DRV_status Can_get_msg(uint8_t *msg) {
    CAN_DRV_status st = CAN_DRV_ST_OK;

    uint8_t i;
    uint8_t dlc = CANCDMOB & (1<<DLC3 & 1<<DLC2 & 1<<DLC1 & 1<<DLC0);

    for (i = 0; i < dlc; i++) {
        *(msg+i) = CANMSG;
    }

    return st;
}
