#pragma once
#include <avr/io.h>

#define CAN_NUM_MOBS (6)
#define MOB_0 (0x00u)
#define MOB_1 (0x01u)
#define MOB_2 (0x02u)
#define MOB_3 (0x03u)
#define MOB_4 (0x04u)
#define MOB_5 (0x05u)
#define MOB_6 (0x06u)
#define NO_MOB (0xFFu)
#define CAN_DLC_MASK (0xF)

/*
 * Macros for register manipulation
 */
#define can_set_mob(mob) (CANPAGE = mob << MOBNB0)
#define can_clear_mob_st (CANSTMOB = 0x00)
#define can_set_mob_id(id) \
    do {                   \
        CANIDT1 = id >> 3; \
        CANIDT2 = id << 5; \
        CANIDT3 = 0x00;    \
        CANIDT4 = 0x00;    \
    } while (0);
#define can_set_mob_mask(mask)                 \
    do {                                       \
        CANIDM1 = mask >> 3;                   \
        CANIDM2 = mask << 5;                   \
        CANIDM3 = 0x00;                        \
        CANIDM4 = (_BV(RTRMSK) | _BV(IDEMSK)); \
    } while (0);
#define can_set_dlc(length) (CANCDMOB |= length << DLC0)
#define can_cfg_rx() (CANCDMOB |= 0x02 << CONMOB0)
#define can_cfg_tx() (CANCDMOB |= 0x01 << CONMOB0)
#define can_clear_mob_status() (CANSTMOB = 0x00)

#define MOB_ST_NOT_COMPLETE (0x00)
#define MOB_ST_RX_OK (1 << RXOK)
#define MOB_ST_RX_OK_DLCW ((1 << RXOK) | (1 << DLCW))
#define MOB_ST_TX_OK (1 << TXOK)
#define MOB_ST_BIT_ERROR (1 << BERR)
#define MOB_ST_STUFF_ERROR (1 << SERR)
#define MOB_ST_CRC_ERROR (1 << CERR)
#define MOB_ST_FORM_ERROR (1 << FERR)
#define MOB_ST_DISABLED (0xFF)

/*
 * Initialize CAN driver
 */
void can_init(void);

/*
 * Returns number of first free message object
 */
uint8_t can_get_free_mob(void);

/*
 * Retrieves data from CANPAGE
 */
void can_get_data(uint8_t* msg, uint8_t dlc);

/*
 * Gets the status of the MOb
 *
 * returns: TODO
 *   MOB_NOT_COMPLETE - Data still in motion
 *   MOB_RX_OK        - Message received
 *   MOB_TX_OK        - Message sent
 *   Anything else    - Error
 */
uint8_t can_get_mob_status(uint8_t mob);
