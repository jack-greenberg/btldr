#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "can_isp.h"
#include "can_lib.h"
#include "config.h"  // TODO: Btldr version should be from eeprom
#include "flash.h"
#include "image.h"

static struct SessionData session = {
    .is_active = false,
    .type = 0,
    .current_addr = {0},
    .remaining_size = {0},
};

// Temporary page buffer
static uint8_t temp_buffer[PAGESIZE_B] = { 0 };
static uint8_t buffer_size = 0;

/*************************************************
 * Handler functions defined in can_isp_commands.c
 **************************************************/

uint8_t handle_query(uint8_t* data, uint8_t length) {
    uint8_t st = 0;

    Can_msg_t response = {
        .mob = CAN_AUTO_MOB,
        .mask = 0x00,
        .id = CAN_ID_QUERY_RESPONSE,
    };

    // TODO:
    //   Get bootloader version from EEPROM
    uint8_t version = ((BOOTLOADER_VERSION_MAJ & 0xF) << 4)
                      | (BOOTLOADER_VERSION_MIN & 0xF);

    // TODO: Use getter function
    uint8_t chip = CHIP_AVR_ATMEGA16M1;

    uint8_t response_data[] = {version, chip};  // TODO: What else?
    response.data = response_data;
    response.length = 2;

    st = can_transmit(&response);

    return st;
}

uint8_t handle_reset(uint8_t* data, uint8_t length) {
    uint8_t st = 0;

    // Validate image
    const image_hdr_t* image_hdr = image_get_header();
    uint8_t valid = image_validate(image_hdr);

    if (valid == IMAGE_VALID) {
        // Indicate jumping with CAN message?
        // Clear bootflag
        jump_to_app();
    } else {
        // Transmit error with invalid image and reason for invalid
        uint8_t data[2] = {ERR_IMAGE_INVALID, valid};
        Can_msg_t response = {
            .mob = CAN_AUTO_MOB,
            .mask = CAN_NO_FILTERING,
            .id = CAN_ID_ERROR,
            .data = data,
            .length = 2,
        };

        st = can_transmit(&response);
    }

    // Unreachable if image is valid
    return st;
}

uint8_t handle_request(uint8_t* data, uint8_t length) {
    uint8_t st = 0;
    if (session.is_active) {
        uint8_t data[5] = {
            ERR_SESSION_EXISTS,
            session.current_addr.bytes[0],
            session.current_addr.bytes[1],
            session.remaining_size.bytes[0],
            session.remaining_size.bytes[1],
        };

        Can_msg_t msg = {
            .mob = CAN_AUTO_MOB,
            .mask = CAN_NO_FILTERING,
            .id = CAN_ID_ERROR,
            .data = data,
            .length = 4,
        };
        st = can_transmit(&msg);
        goto bail;
    }

    session.is_active = true;
    session.type = data[0];

    if (data[0] == REQUEST_DOWNLOAD) {
        session.current_addr.word = 0;
        session.remaining_size.bytes[0] = data[1];
        session.remaining_size.bytes[1] = data[2];
    } else if (data[0] == REQUEST_UPLOAD) {
        session.current_addr.word = 0;
        session.remaining_size.word = 0;  // Get image size from header
    } else {
    }

bail:
    return st;
}

uint8_t handle_data(uint8_t* data, uint8_t length) {
    uint8_t st = 0;
    if (!session.is_active) {
        uint8_t data[1] = {
            ERR_NO_SESSION,
        };

        Can_msg_t msg = {
            .mob = CAN_AUTO_MOB,
            .mask = CAN_NO_FILTERING,
            .id = CAN_ID_ERROR,
            .data = data,
            .length = 1,
        };
        st = can_transmit(&msg);
        goto bail;
    }

    // Write data to temporary buffer

bail:
    return st;
}
