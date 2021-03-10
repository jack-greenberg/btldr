#include <avr/io.h>
#include "can.h"

inline void Can_init(void) {
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
        CANIDT4 = 0x00;
        CANIDT3 = 0x00;
        CANIDT2 = 0x00;
        CANIDT1 = 0x00;
        CANIDM4 = 0x00;
        CANIDM3 = 0x00;
        CANIDM2 = 0x00;
        CANIDM1 = 0x00;
    }

    // Re-enable CAN
    CANGCON = 1 << ENASTB;
}
