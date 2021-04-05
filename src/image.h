#pragma once
#include <stdint.h>

#define IMAGE_MAGIC (0x0E13)  // OEM

typedef struct {
    uint16_t image_magic;
    uint32_t crc;
    uint32_t image_size;
    // uint8_t image_type;
    // uint8_t version_major;
    // uint8_t version_minor;
    // uint8_t version_patch;
    uint32_t vector_addr;
    uint32_t reserved;
    char git_sha[8];
} image_hdr_t;

const image_hdr_t *image_get_header(void);

int image_validate(const image_hdr_t *hdr);
