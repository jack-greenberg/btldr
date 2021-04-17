#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#include "../src/config.h" // TODO: Shouldn't have to reference like this
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

CAN_isp_status can_node_select(uint8_t *data, uint8_t length) {
    Can_msg resp = {
        .mob = CAN_AUTO_MOB,
        .mask = 0x00,
        .id = CAN_ID_SESSION_START,
    };

    uint8_t version = ((BOOTLOADER_VERSION_MAJ & 0xF) << 4) \
                      | (BOOTLOADER_VERSION_MIN & 0xF);

    uint8_t resp_data[2] = { 0x00, version };

    if (data[0] == NODE_SELECT_OPEN) {
        is_programming = true;
        resp_data[0] = 0x01;
    }

    resp.data = (uint8_t *) resp_data;
    resp.length = 2;

    can_transmit(&resp);

    // TODO should the device jump back to the app?

    return CAN_ISP_ST_OK;
}

CAN_isp_status can_session_start(uint8_t *data, uint8_t length) {
    uint16_t start_addr = (data[1] << 8) | data[2];
    uint16_t end_addr = (data[3] << 8) | data[4];
    program_memory_address = start_addr;
    program_memory_num_bytes = (end_addr - start_addr) + 1;

    uint8_t resp_data[1] = { RESP_SESSION_OK };
    Can_msg resp = {
        .mob = CAN_AUTO_MOB,
        .mask = 0x00,
        .id = CAN_ID_SESSION_START,
        .length = 1,
        .data = (uint8_t *) resp_data,
    };

    can_transmit(&resp);
    return CAN_ISP_ST_OK;
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

    Can_msg resp = {
        .mob = CAN_AUTO_MOB,
        .mask = 0x00,
        .id = CAN_ID_DATA,
        .length = 1,
    };

    uint8_t resp_data[1];
    if (program_memory_num_bytes != 0) {
        resp_data[0] = RESP_DATA_OK;
    } else {
        resp_data[0] = RESP_DATA_EOF;
    }

    resp.data = (uint8_t*) resp_data;
    can_transmit(&resp);

    return CAN_ISP_ST_OK;
}

CAN_isp_status can_start_app(uint8_t* msg, uint8_t length) {
    image_hdr_t *img_hdr = image_get_header();

    Can_msg resp = {
        .id = CAN_ID_START_APP,
        .mask = 0x00,
        .mob = CAN_AUTO_MOB,
        .length = 1,
    };

    bool valid;
    uint8_t resp_data[1];
    if (!image_validate(img_hdr)) {
        valid = false;
        resp_data[0] = RESP_START_IMAGE_INVALID;
    } else {
        valid = true;
        resp_data[0] = RESP_START_OK;
    }

    resp.data = resp_data;

    can_transmit(&resp);
    
    if (valid) {
        return CAN_ISP_ST_START_APP;
    } else {
        return CAN_ISP_ST_ERROR;
    }
}
