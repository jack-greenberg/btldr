#pragma once

/*
 * CAN ISP Commands
 */
#define CAN_ID_NODE_SELECT (0x03u)
#define CAN_ID_PROG_START (0x04u)
#define CAN_ID_DATA (0x05u)
#define CAN_ID_READ (0x06u)
#define CAN_ID_START_APP (0x07u)
#define CAN_ID_VALIDATE (0x08u)
#define CAN_ID_VERSION (0x09u)

/*
 * Other defines
 */
#define CAN_ISP_MASK (0xFF)
#define CAN_MAX_MSG_LENGTH (8)

// TODO: Node number management
#define NODE_ID 0x1

typedef enum {
    CAN_ISP_ST_SUCCESS,
    CAN_ISP_ST_ERROR,
} CAN_isp_status;

CAN_isp_status can_isp_task(void);
