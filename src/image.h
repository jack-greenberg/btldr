#pragma once
#include <stdint.h>

#define IMAGE_MAGIC (0xCAFE)  // OEM

typedef struct __attribute__((packed)) {
    uint16_t image_magic;
    uint32_t crc; // TODO: 16 or 32 bit CRC?
    uint16_t image_size;
    uint64_t flash_timestamp;
    uint32_t reserved;
    char git_sha[8];
} image_hdr_t;

const image_hdr_t* image_get_header(void);

int image_validate(const image_hdr_t *hdr);
