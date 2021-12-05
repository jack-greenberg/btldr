#include "commands.h"

char* chip_id_to_name[4] = {
    [CHIP_NONE] = "unknown",
    [CHIP_AVR_ATMEGA16M1] = "atmega16m1",
    [CHIP_AVR_ATMEGA328P] = "atmega328p",
    [CHIP_ARM_STM32F103C8T6] = "stm32f103c8t6",
};

int cmd_ping(can_client_t* client, uint8_t ecu_id, uint8_t* current_image) {
    int rc = 0;

    // Receive response
    uint16_t recv_can_id;
    uint8_t recv_can_dlc;
    uint8_t recv_can_data[8];

    int num_tries = 0;

    uint64_t current_time = 0;

    struct can_filter rfilter[1] = {0};

    if (ecu_id == PING_BROADCAST) {
        rfilter[0].can_id = CAN_ID_QUERY_RESPONSE;
        rfilter[0].can_mask = 0x00F;  // 0bxxxxxxx0001 to match ping responses
    } else {
        rfilter[0].can_id = (ecu_id << 4) | CAN_ID_QUERY_RESPONSE;
        rfilter[0].can_mask = 0x7FF;
    }

    do {
        // Send ping command
        uint16_t can_id = (ecu_id << 4) | CAN_ID_QUERY;

        // Get current time to transmit in ping
        current_time = time(NULL);
        uint8_t can_time[8];
        memcpy(can_time, &current_time, sizeof(uint64_t));

        rc = can_send(client, can_id, can_time, sizeof(uint64_t));
        if (rc != 0) {
            goto bail;
        }

        rc = can_receive(client, rfilter, &recv_can_id, &recv_can_dlc,
                         (uint8_t*)recv_can_data, POLLTIMEOUT);
        printf("PING 0x%X\n", can_id);
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

    // TODO match received ID to board somehow?
    uint8_t version = recv_can_data[0];
    uint8_t version_maj = (version & 0xF0) >> 4;
    uint8_t version_min = (version & 0x0F);
    char* chip = chip_id_to_name[recv_can_data[1]];
    *current_image = recv_can_data[2];

    char* image_name = (*current_image == CURRENT_IMAGE_APP) ? "app" : "updater";

    uint32_t time_delta;
    memcpy(&time_delta, recv_can_data + 4, sizeof(uint32_t));

    const time_t timer = current_time - time_delta;
    struct tm flash_time;
    (void)gmtime_r(&timer, &flash_time);

    char flash_time_str[64];
    (void)strftime(flash_time_str, 64, "%x %H:%M", &flash_time);

    printf("PING 8 bytes from 0x%X: chip=%s version=%i.%i image=%s flashed=%s\n",
           ecu_id, chip, version_maj, version_min, image_name, flash_time_str);

bail:
    return rc;
}
