#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#include "can_isp.h"
#include "can_lib.h"
#include "flash.h"
#include "image.h"

extern bool is_programming;

// Current (unprogrammed) memory address
static uint16_t program_memory_address;

// Number of bytes remaining to program
static uint16_t program_memory_num_bytes;

// Temporary page buffer
static uint8_t temp_buffer[PAGESIZE_B] = {0xFF};
static uint8_t buffer_size = 0;

CAN_isp_status can_node_select(uint8_t *data) {
    if (data[0] == NODE_ID) {
        is_programming = true;
    } else {
        // TODO What does the ECU do if it's not being updated?
    }
}

CAN_isp_status can_session_start(uint8_t *data) {
    uint16_t start_addr = (data[1] << 8) | data[2];
    uint16_t end_addr = (data[3] << 8) | data[4];
    program_memory_address = start_addr;
    program_memory_num_bytes = (end_addr - start_addr) + 1;
}

CAN_isp_status can_data(uint8_t *data, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        temp_buffer[buffer_size] = *data++;

        buffer_size++;
        program_memory_num_bytes--;
        program_memory_address++;
    }

    if (buffer_size >= PAGESIZE_B || program_memory_num_bytes == 0) {
        uint16_t page_address =
            program_memory_address - (program_memory_address % PAGESIZE_B);
        flash_write(temp_buffer, page_address);
    }
}

CAN_isp_status can_start_app() {
    image_hdr_t *img_hdr = image_get_header();
    if (img_hdr == NULL) {
        // TODO: Error handling
    }

    bool valid = image_validate(img_hdr);

    if (!valid) {
        // TODO: Error handling
    } else {
        start_app();
    }
}
