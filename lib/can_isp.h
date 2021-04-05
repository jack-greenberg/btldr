#pragma once

/*
 * CAN ISP Commands
 */

typedef enum {
    CAN_ISP_ST_SUCCESS,
    CAN_ISP_ST_ERROR,
} CAN_isp_status;

// TODO Determine final command structure
typedef enum {
    NO_OPERATION = 0x00u,
    SELECT_NODE = 0x01u,
    PROG_START = 0x02u,
    DATA = 0x03u,
    READ = 0x04u,
    START_APP = 0x05u,
    ERROR = 0xFFu,
} command_t;
