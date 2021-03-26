#include <avr/io.h>
#include "can.h"
#include <stdint.h>
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

// void can_clear_all_mob(void) {
//     uint8_t mob;
//     for (mob = 0; mob < CAN_NUM_MOBS; mob++) {
//         CANPAGE = (mob << 4);
//         CANSTMOB = 0x00;
//         CANCDMOB = 0x00;
//     }
// }
