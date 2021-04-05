#include "can_isp.h"

#include <stdint.h>

// Concept: polling vs interrupts
//      in polling, you check to see if something is done:
//          can_wait_on_receive()
//      with interrupts, you jump to an ISR whenever there
//      is an interrupt
//
// We will use polling
CAN_ISP_status can_isp_task(void) {
    CAN_ISP_status st = CAN_ISP_ST_SUCCESS;

    // switch () {
    // }

    return st;
}
