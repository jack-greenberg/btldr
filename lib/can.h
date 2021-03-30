#include <stdint.h>

#define CAN_NUM_MOBS (6)

typedef enum {
    CAN_DRV_ST_OK,
    CAN_DRV_ST_ERROR,
    CAN_DRV_ST_MOB_BUSY,
} CAN_DRV_status;

/*
 * Initialize CAN driver
 */
inline void Can_init(void);

CAN_DRV_status Can_wait_on_receive(uint16_t id, uint16_t mask, uint8_t msg_length);

CAN_DRV_status Can_get_msg(uint8_t* msg);

typedef enum {
    CAN_ST_OK,
    CAN_ST_INVALID_PARAM,
    CAN_ST_ERROR,
} CAN_status;

typedef enum {
    CAN_TX,
    CAN_RX,
} Can_cmd_t;

typedef struct {
    Can_cmd_t cmd;
    uint16_t msk;
    uint16_t id;
    uint8_t length;
    uint8_t* data;
    CAN_status st;
} Can_action_t;

CAN_status Can_cmd(Can_action_t action);
