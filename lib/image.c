#include "image.h"

#include <avr/pgmspace.h>
#include <stdint.h>

#include "crc32.h"

// Address of image header. To use, cast as void *
extern int __image_hdr;

// Static variable to store image header in memory
static image_hdr_t prv_hdr;

const image_hdr_t* image_get_header(void) {
    memcpy_P(&prv_hdr, (void*)__image_hdr, sizeof(image_hdr_t));
    return &prv_hdr;
}

uint8_t image_validate(const image_hdr_t* hdr) {
    // Get image start address (after header) and size
    uint16_t image_addr = sizeof(image_hdr_t);
    uint16_t image_size = hdr->image_size;

    // Image must start with `OEM!`
    if (hdr->image_magic != IMAGE_MAGIC) {
        return IMAGE_INVALID_MAGIC;
    }

    // Calculate CRC32 of image
    uint32_t crc = ~0x0;
    uint8_t data = 0x00;
    for (; image_size > 0; image_size--, image_addr++) {
        data = pgm_read_byte(image_addr);
        crc32_step(data, &crc);
    }
    crc = ~crc;

    // Calculated CRC is not the same as the stored CRC
    if (crc != hdr->crc) {
        return IMAGE_INVALID_CRC;
    }

    return IMAGE_VALID;
}

uint64_t image_get_timestamp(void) {
    const image_hdr_t* img = image_get_header();
    return (uint64_t)img->flash_timestamp;
}

uint16_t image_get_size(void) {
    const image_hdr_t* img = image_get_header();
    return (uint16_t)img->image_size;
}
