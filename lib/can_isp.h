#pragma once
#include <stdint.h>

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
#define CAN_ISP_MASK (0x3FF)
#define CAN_MAX_MSG_LENGTH (8)

// TODO: Node number management
#define NODE_ID 0x1

typedef enum {
    CAN_ISP_ST_SUCCESS,
    CAN_ISP_ST_ERROR,
} CAN_isp_status;

CAN_isp_status can_isp_task(void);

void (* start_app)(void);

// Private functions defined in can_isp_commands.c
void can_node_select(uint8_t* data);
void can_prog_start(uint8_t* data);
void can_data(uint8_t* data, uint8_t length);
void can_read(uint8_t* data);
void can_start_app(uint8_t* data);
void can_validate(uint8_t* data);
void can_version(uint8_t* data);
