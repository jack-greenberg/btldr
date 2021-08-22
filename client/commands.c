#include "commands.h"

#include "can_client.h"
#include "can_isp.h"
#include "log.h"
#include "image.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int cmd_flash(uint8_t ecu_id, char* binary_path) {
    log_info("Flashing id 0x%X with binary at %s", ecu_id, binary_path);
    return 0;
}

int cmd_ping(uint8_t ecu_id) {
    log_info("Pinging id 0x%X", ecu_id);

    uint16_t can_filter_id = ecu_id << 4;
    uint16_t can_filter_mask = 0x7FE; // Only receive the next inc. message
    int rc = init_can_client(can_filter_id, can_filter_mask);
    if (rc != 0) {
        goto bail;
    }

    // Send ping command
    uint8_t can_id = (ecu_id << 4) | CAN_ID_QUERY;
    rc = can_send(can_id, NULL, 0);
    if (rc != 0) {
        goto bail;
    }

    // Receive response
    uint8_t recv_can_id, recv_can_dlc;
    uint8_t recv_can_data[8];
    rc = can_receive(&recv_can_id, &recv_can_dlc, (uint8_t*)recv_can_data);

    uint8_t version = recv_can_data[0];
    uint8_t version_maj = (version & 0xF0) >> 4;
    uint8_t version_min = (version & 0x0F);
    char* chip = chip_id_to_name[recv_can_data[1]];

    printf("Found chip_id=%s with updater version %i.%i\n", chip, version_maj, version_min);

bail:
    can_client_destroy();
    return rc;
}
