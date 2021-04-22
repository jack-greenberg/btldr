#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#include "config.h" // TODO: Shouldn't have to reference like this
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

    switch (data[0]) {
        case NODE_SELECT_OPEN: {
            if (is_programming) {
                is_programming = false;
                resp_data[0] = RESP_SELECT_CLOSED;
            } else {
                is_programming = true;
                resp_data[0] = RESP_SELECT_OPENED;
            }
            break;
        }
        case NODE_SELECT_QUERY: {
            resp_data[0] = is_programming ? \
                           RESP_SELECT_OPENED : RESP_SELECT_CLOSED;
            break;
        }
    }

    resp.data = resp_data;
    resp.length = 2;

    can_transmit(&resp);

    return CAN_ISP_ST_OK;
}

CAN_isp_status can_session_start(uint8_t *data, uint8_t length) {
    program_memory_address = 0x00;
    program_memory_num_bytes = (data[1] << 8) | data[2];

    switch (data[0]) {
        case SESSION_UPLOAD: {
            uint8_t msg[1] = { 0x00 };
            Can_msg resp = {
                .mob = CAN_AUTO_MOB,
                .mask = 0x00,
                .id = CAN_ID_SESSION_START,
                .length = 1,
                .data = msg,
            };

            can_transmit(&resp);
            break;
        }
        case SESSION_DOWNLOAD: {
            const image_hdr_t *hdr = image_get_header();
            uint16_t image_size = hdr->image_size;  // TODO: 32 bits or 16?
            uint8_t msg[8];

            for (uint16_t i = image_size; i >= 0; i-= 8) {
                uint16_t addr = image_size - i;
                uint16_t size = i % 8 == 0 ? 8 : i % 8;

                flash_read(addr, msg, size);

                Can_msg resp = {
                    .mob = CAN_AUTO_MOB,
                    .mask = 0x00,
                    .id = CAN_ID_SESSION_START,
                    .length = size,
                    .data = msg,
                };

                can_transmit(&resp);
            }
        }
        default: {
            break;
        }
    }

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

        // Reset buffer size
        buffer_size = 0;
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

    resp.data = resp_data;
    can_transmit(&resp);

    return CAN_ISP_ST_OK;
}

CAN_isp_status can_start_app(uint8_t* msg, uint8_t length) {
    CAN_isp_status st;
    const image_hdr_t *img_hdr = image_get_header();

    Can_msg resp = {
        .id = CAN_ID_START_APP,
        .mask = CAN_NO_FILTERING,
        .mob = CAN_AUTO_MOB,
        .length = 1,
    };

    uint8_t resp_data[1];
    if (!image_validate(img_hdr)) { // TODO: Remove `!`
        resp_data[0] = RESP_START_IMAGE_INVALID;
        st = CAN_ISP_ST_ERROR;
    } else {
        resp_data[0] = RESP_START_OK;
        st = CAN_ISP_ST_OK;
    }

    resp.data = resp_data;
    can_transmit(&resp);
    return st;
}
