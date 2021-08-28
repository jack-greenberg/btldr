#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "can_isp.h"
#include "can_lib.h"
#include "flash.h"
#include "image.h"
#include "shared_mem.h"

static struct session_data session = {
    .is_active = false,
    .type = 0,
    .current_addr = {0},
    .remaining_size = {0},
};

// Temporary page buffer
// TODO: Use boot_page_fill instead of this temp buffer so that we don't need an
// additional buffer
// static uint8_t temp_buffer[PAGESIZE_B] = { 0 };
// static uint8_t buffer_size = 0;

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

    uint8_t version = updater_get_version();

    // TODO: Use getter function
    uint8_t chip = CHIP_AVR_ATMEGA16M1;

    uint8_t response_data[] = {version, chip};
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
        bootflag_clear(UPDATE_REQUESTED);
        bootflag_set(IMAGE_IS_VALID);

        // Back to bootloader
        asm("jmp 0x3000"); // TODO How to get bootstart address?
    } else {
        // Transmit error with invalid image and reason for invalid
        uint8_t err_data[2] = {ERR_IMAGE_INVALID, valid};
        Can_msg_t response = {
            .mob = CAN_AUTO_MOB,
            .mask = CAN_NO_FILTERING,
            .id = CAN_ID_ERROR,
            .data = err_data,
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
        uint8_t err_data[5] = {
            // ERR_SESSION_EXISTS,
            0, // TODO
            session.current_addr.bytes[0],
            session.current_addr.bytes[1],
            session.remaining_size.bytes[0],
            session.remaining_size.bytes[1],
        };

        Can_msg_t msg = {
            .mob = CAN_AUTO_MOB,
            .mask = CAN_NO_FILTERING,
            .id = CAN_ID_ERROR,
            .data = err_data,
            .length = 5,
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
        uint8_t err_data[1] = {
            ERR_INVALID_COMMAND,
        };

        Can_msg_t msg = {
            .mob = CAN_AUTO_MOB,
            .mask = CAN_NO_FILTERING,
            .id = CAN_ID_ERROR,
            .data = err_data,
            .length = 4,
        };
        st = can_transmit(&msg);
    }

bail:
    return st;
}

uint8_t handle_data(uint8_t* data, uint8_t length) {
    uint8_t st = 0;
    if (!session.is_active) {
        uint8_t err_data[1] = {
            // ERR_NO_SESSION,
            0 // TODO
        };

        Can_msg_t msg = {
            .mob = CAN_AUTO_MOB,
            .mask = CAN_NO_FILTERING,
            .id = CAN_ID_ERROR,
            .data = err_data,
            .length = 1,
        };
        st = can_transmit(&msg);
        goto bail;
    }

    // Write data to temporary buffer
    flash_write(data, length, &(session.current_addr.word));

    session.remaining_size.word -= length;

    // Status update
    uint8_t status_data[4] = {
        session.current_addr.bytes[0],
        session.current_addr.bytes[1],
        session.remaining_size.bytes[0],
        session.remaining_size.bytes[1],
    };

    Can_msg_t msg = {
        .mob = CAN_AUTO_MOB,
        .mask = CAN_NO_FILTERING,
        .id = CAN_ID_STATUS,
        .data = status_data,
        .length = 4,
    };
    st = can_transmit(&msg);

bail:
    return st;
}
