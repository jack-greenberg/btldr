#pragma once
#include <stdbool.h>
#include <stdint.h>

#define IMAGE_MAGIC (0x4F454D21)  // "OEM!"

#define IMAGE_VALID (0x0)
#define IMAGE_INVALID_CRC (0x1)
#define IMAGE_INVALID_MAGIC (0x2)

typedef struct __attribute__((packed)) {
    uint32_t image_magic;
    uint32_t crc;  // TODO: 16 or 32 bit CRC?
    uint16_t image_size;
    uint64_t flash_timestamp;
    uint32_t reserved;
    char git_sha[8];
} image_hdr_t;

const image_hdr_t* image_get_header(void);

int image_validate(const image_hdr_t* hdr);
