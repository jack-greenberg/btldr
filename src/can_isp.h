#pragma once
#include <stdbool.h>
#include <stdint.h>

/********************
 * CAN ISP Commands *
 ********************/

/*
 * client -> target
 *
 * DLC=8
 * [0..7] => 64-bit current unix timestamp.
 */
#define CAN_ID_QUERY (0x000U)

/*
 * client -> target
 *
 * DLC=1
 * [0] => 0x1 to request update
 */
#define CAN_ID_RESET (0x002U)
#define RESET_REQUEST_UPDATE (0x1)

/*
 * client -> target
 *
 * DLC = (upload) 3, (download) 1
 * [0]    => 0x0 for download, 0x1 for upload (upload == flash)
 * [1..2] => (upload-only) 16 bit image size
 */
#define CAN_ID_REQUEST (0x004U)  // Upload or download
#define REQUEST_DOWNLOAD (0x000U)
#define REQUEST_UPLOAD (0x001U)

/*
 * client -> target
 *
 * DLC = 1-8
 * [0..dlc] => Program data to be flashed
 */
#define CAN_ID_DATA (0x006U)

/*************
 * Responses *
 *************/

/*
 * target -> client
 *
 * DLC = 8
 *
 * [0]    => bootloader version (MMMM.mmmm bits)
 * [1]    => chip ID
 * [2..3] => reserved
 * [4..7] => Time delta from query unix timestamp and flash timestamp
 */
#define CAN_ID_QUERY_RESPONSE (0x001U)

/*
 * client -> target
 *
 * DLC = 1-8
 * [0]      => Error code (below)
 * [1..dlc] => Error details
 */
#define CAN_ID_ERROR (0x003U)

// Error codes
#define ERR_INVALID_COMMAND (0x000U)  // Command unknown
// #define ERR_NO_SESSION (0x001U) // Data sent but session isn't active
// #define ERR_TIMEOUT (0x002U) //
// #define ERR_SESSION_EXISTS (0x003U) // Session command repeated
#define ERR_IMAGE_INVALID (0x001U)

/*
 * client -> target
 *
 * DLC = 4
 * [0..1] => Last programmed address
 * [2..3] => Remaining data
 */
#define CAN_ID_STATUS (0x005U)  // Sends status messages during update

/*
 * Other defines
 */
#define CAN_ISP_MASK (0x000)    // No filtering
#define CAN_MAX_MSG_LENGTH (8)  // Always accept up to 8 bytes

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
