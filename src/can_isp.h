#pragma once
#include <stdbool.h>
#include <stdint.h>

/*
 * CAN ISP Commands
 *
 * TODO: Support multiple CAN messages, one for each ECU, one for tester?
 */
#define CAN_ID_QUERY (0x000U)
#define CAN_ID_RESET (0x002U)
#define CAN_ID_REQUEST (0x004U)  // Upload or download
#define CAN_ID_DATA (0x006U)

#define REQUEST_DOWNLOAD (0x000U)
#define REQUEST_UPLOAD (0x001U)

/*
 * Responses
 */
#define CAN_ID_QUERY_RESPONSE (0x001U)
#define CAN_ID_ERROR (0x003U)
#define CAN_ID_SESSION (0x005U)
#define CAN_ID_STATUS (0x007U)  // Sends status messages during update

/*
 * Error codes
 */
#define ERR_INVALID_COMMAND (0x000U)
#define ERR_NO_SESSION (0x001U)
#define ERR_TIMEOUT (0x002U)
#define ERR_SESSION_EXISTS (0x003U)
#define ERR_IMAGE_INVALID (0x004U)

/*
 * Other defines
 */
#define CAN_ISP_MASK (0x000)    // No filtering
#define CAN_MAX_MSG_LENGTH (8)  // Always accept up to 8 bytes

/**
 * Jumps to application
 *
 * This macro does NOT do image validation, and should be paired with a
 * function that checks the validity of the image.
 */
#define jump_to_app() asm("jmp %0" ::"I"(sizeof(image_hdr_t)));

union Address {
    uint16_t word;
    uint8_t bytes[2];
};

struct session_data {
    bool is_active;  // Is in a session (upld or dwld)
    uint8_t type;    // 0 for download, 1 for upload
    union {
        // Current unprogrammed address
        uint16_t word;
        uint8_t bytes[2];
    } current_addr;
    union {
        uint16_t word;
        uint8_t bytes[2];
        // Remaining size to download or upload
    } remaining_size;
};

/**
 * Main CAN message task
 *
 * Sets up CAN listening, handles responses and processes them using one of the
 * functions below. Also sends responses to the host.
 *
 * @see
 * @returns
 *   0 - success
 *   1 - Unrecoverable error
 */
uint8_t can_isp_task(void);

/*************************************************
 * Handler functions defined in can_isp_commands.c
 **************************************************/

/**
 * Handles query request. Checks to see if node id matches, and if so, returns a
 * message with the bootloader version and flash timestamp
 *
 * TODO:
 *   Flash timestamp is 8 bytes, so need 2 CAN messages?
 */
uint8_t handle_query(uint8_t* data, uint8_t length);

/**
 * Handles request to reset the ECU. Should NOT return unless there is an error
 */
uint8_t handle_reset(uint8_t* data, uint8_t length);

/**
 * Handles request to upload or download software. Upload means the device
 * should send it's flash contents to the host. Download means the device should
 * receive data from the host and update it's flash.
 */
uint8_t handle_request(uint8_t* data, uint8_t length);

/**
 * Handles program data coming in from the host. Responsible for sending STATUS
 * messages to relay the progress and status of the update
 */
uint8_t handle_data(uint8_t* data, uint8_t length);
