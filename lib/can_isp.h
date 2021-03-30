#include <stdint.h>

/*
 * CAN ISP Commands
 */
// struct CAN_ISP_command {
// 
// }
// #define CAN_ISP_GET_VERSION (0x00)
// #define CAN_ISP_GET_ID (0x01)
// #define CAN_ISP_SELECT_NODE (0x02)
// #define CAN_ISP_READ_MEMORY (0x10)
// #define CAN_ISP_WRITE_MEMORY (0x11)
// #define CAN_ISP_GO (0x20)
// #define CAN_ISP_ERASE (0x30)
// #define CAN_ISP_WRITE_PROTECT (0x40)
// #define CAN_ISP_WRITE_UNPROTECT (0x41)

typedef enum {
    CAN_ISP_SUCCESS,
    CAN_ISP_ERROR,
} CAN_ISP_status;
