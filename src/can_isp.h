#pragma once
#include <stdint.h>

/*
 * CAN ISP Commands
 */
#define CAN_ID_NODE_SELECT (0x03u)
#define NODE_SELECT_OPEN (0x00)
#define NODE_SELECT_QUERY (0x80)

#define CAN_ID_SESSION_START (0x04u)
#define SESSION_UPLOAD (0x00)
#define SESSION_DOWNLOAD (0x80)

#define CAN_ID_DATA (0x05u)

#define CAN_ID_START_APP (0x07u)

/*
 * CAN ISP Responses
 */
#define RESP_DATA_OK (0x01)
#define RESP_DATA_EOF (0x00)
#define RESP_SELECT_CLOSED (0x00)
#define RESP_SELECT_OPENED (0x01)
#define RESP_SESSION_OK (0x00)
#define RESP_START_OK (0x00)
#define RESP_START_IMAGE_INVALID (0x01)

/*
 * Other defines
 */
#define CAN_ISP_MASK (0x000)  // No filtering
#define CAN_MAX_MSG_LENGTH (8)  // Always accept up to 8 bytes

typedef enum {
    CAN_ISP_ST_OK,
    CAN_ISP_ST_ERROR,
    CAN_ISP_ST_START_APP,
} CAN_isp_status;

CAN_isp_status can_isp_task(void);

void (*start_app)(void);

// Private functions defined in can_isp_commands.c
CAN_isp_status can_node_select(uint8_t *data, uint8_t length);
CAN_isp_status can_session_start(uint8_t *data, uint8_t length);
CAN_isp_status can_data(uint8_t *data, uint8_t length);
CAN_isp_status can_start_app(uint8_t *msg, uint8_t length);
