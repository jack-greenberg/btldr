#include "commands.h"

#include <linux/can.h>
#include <linux/can/raw.h>
#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "can_client.h"
#include "can_isp.h"
#include "image.h"
#include "log.h"

#define POLLTIMEOUT (1000)
#define MAX_RETRIES (5)
#define PING_BROADCAST (0xFF)

char* chip_id_to_name[4] = {
    [CHIP_NONE] = "unknown",
    [CHIP_AVR_ATMEGA16M1] = "atmega16m1",
    [CHIP_AVR_ATMEGA328P] = "atmega328p",
    [CHIP_ARM_STM32F103C8T6] = "CHIP_ARM_STM32F103C8T6",
};

int cmd_flash(uint8_t ecu_id, char* binary_path) {
    log_info("Flashing id 0x%X with binary at %s", ecu_id, binary_path);
    return 0;
}

int cmd_ping(uint8_t ecu_id) {
    uint16_t can_filter_id;
    uint16_t can_filter_mask;

    if (ecu_id == PING_BROADCAST) {
        can_filter_id = 0x001;
        can_filter_mask = 0x00F;  // 0bxxxxxxx0001 to match ping responses
    } else {
        can_filter_id = (ecu_id << 4) & 0x1;
        can_filter_mask = 0x7FF;
    }

    int rc = init_can_client();
    if (rc != 0) {
        goto bail;
    }

    // Receive response
    uint16_t recv_can_id;
    uint8_t recv_can_dlc;
    uint8_t recv_can_data[8];

    int num_tries = 0;

    do {
        // Send ping command
        uint8_t can_id = (ecu_id << 4) | CAN_ID_QUERY;
        rc = can_send(can_id, NULL, 0);
        if (rc != 0) {
            goto bail;
        }

        rc = can_receive(can_filter_id, can_filter_mask, &recv_can_id,
                         &recv_can_dlc, (uint8_t*)recv_can_data, POLLTIMEOUT);
        num_tries++;
    } while ((rc == -1) && (num_tries < MAX_RETRIES));

    if ((num_tries == MAX_RETRIES) && (rc == -1)) {
        printf("Failed to receive ping response. Device unreachable.\n");
        rc = 2;  // timeout
        goto bail;
    }

    if (rc != 0) {
        goto bail;
    }

    uint8_t version = recv_can_data[0];
    uint8_t version_maj = (version & 0xF0) >> 4;
    uint8_t version_min = (version & 0x0F);
    char* chip = chip_id_to_name[recv_can_data[1]];

    printf("Found chip_id=%s with updater version %i.%i\n", chip, version_maj,
           version_min);

bail:
    can_client_destroy();
    return rc;
}
