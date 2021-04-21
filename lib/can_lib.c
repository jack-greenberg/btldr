#include "can_lib.h"

#include <avr/io.h>
#include <stdint.h>

#include "can_drv.h"

extern void can_init();

CAN_status can_receive(Can_msg *msg) {
    msg->mob = can_get_free_mob();

    if (msg->mob == NO_MOB) {
        return CAN_ST_ERROR;
    }

    can_set_mob(msg->mob);
    can_set_mob_id(msg->id);
    can_set_mob_mask(msg->mask);
    can_set_dlc(msg->length);

    can_cfg_rx();

    return CAN_ST_OK;
}

// TODO: Forgot to transmit the data
CAN_status can_transmit(Can_msg *msg) {
    if (msg->mob == CAN_AUTO_MOB) {
        msg->mob = can_get_free_mob();
    }

    if (msg->mob == NO_MOB) {
        return CAN_ST_ERROR;
    }

    can_set_mob(msg->mob);
    can_set_mob_id(msg->id);
    can_set_mob_mask(msg->mask);
    can_set_dlc(msg->length);

    can_cfg_tx();

    return CAN_ST_OK;
}

CAN_status can_poll_complete(Can_msg *msg) {
    uint8_t mob_st = can_get_mob_status(msg->mob);

    if (mob_st == MOB_ST_NOT_COMPLETE) {
        return CAN_ST_NOT_READY;
    } else if (mob_st == MOB_ST_RX_OK || mob_st == MOB_ST_RX_OK_DLCW) {
        can_set_mob(msg->mob);
        can_get_data(msg->data, msg->length);
        can_clear_mob_status();
        return CAN_ST_OK;
    } else if (mob_st == MOB_ST_TX_OK) {
        can_set_mob(msg->mob);
        can_clear_mob_status();
        return CAN_ST_OK;
    } else {
        can_set_mob(msg->mob);
        can_clear_mob_status();
        return CAN_ST_ERROR;
    }
}
