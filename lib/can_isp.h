#pragma once
#include <stdint.h>

/*
 * CAN ISP Commands
 */
#define CAN_ID_NODE_SELECT (0x03u)
#define CAN_ID_SESSION_START (0x04u)
#define CAN_ID_DATA (0x05u)
#define CAN_ID_START_APP (0x07u)

/*
 * Other defines
 */
#define CAN_ISP_MASK (0x3FF)
#define CAN_MAX_MSG_LENGTH (8)

// TODO: Node number management
#define NODE_ID 0x1

typedef enum {
    CAN_ISP_ST_SUCCESS,
    CAN_ISP_ST_ERROR,
} CAN_isp_status;

CAN_isp_status can_isp_task(void);

void (*start_app)(void);

// Private functions defined in can_isp_commands.c
CAN_isp_status can_node_select(uint8_t *data, uint8_t length);
CAN_isp_status can_session_start(uint8_t *data, uint8_t length);
CAN_isp_status can_data(uint8_t *data, uint8_t length);
CAN_isp_status can_start_app(uint8_t *msg, uint8_t length);
