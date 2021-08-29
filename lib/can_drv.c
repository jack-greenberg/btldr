#include "can_drv.h"

#include <avr/io.h>
#include <stddef.h>

void can_init(void) {
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

uint8_t can_get_free_mob(void) {
    uint8_t current_mob = CANPAGE;
    for (uint8_t i = 0; i < CAN_NUM_MOBS; i++) {
        can_set_mob(i);
        if ((CANCDMOB & 0xC0) == 0x00) {
            return i;
        }
    }
    CANPAGE = current_mob;
    return NO_MOB;
}

void can_get_data(uint8_t* msg, uint8_t dlc) {
    for (uint8_t i = 0; i < dlc; i++) { *(msg + i) = CANMSG; }
}

void can_send_data(uint8_t* msg, uint8_t dlc) {
    for (uint8_t i = 0; i < dlc; i++) { CANMSG = *(msg + i); }
}

uint8_t can_get_mob_status(uint8_t mob) {
    uint8_t mob_st;
    uint8_t canstmob = CANSTMOB;

    if ((CANCDMOB & 0xC0) == 0x00) {
        return MOB_ST_DISABLED;
    }

    mob_st = canstmob & ((1 << DLCW) | (1 << RXOK) | (1 << TXOK));
    if (mob_st != 0) {
        return mob_st;
    }

    mob_st = canstmob & ((1 << BERR) | (1 << SERR) | (1 << CERR) | (1 << FERR));
    if (mob_st != 0) {
        return mob_st;
    }

    return MOB_ST_NOT_COMPLETE;
}
