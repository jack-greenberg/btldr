#include "image.h"

#include <avr/pgmspace.h>
#include <stdint.h>

#include "crc32.h"

extern int __image_hdr;

static image_hdr_t hdr;

const image_hdr_t *image_get_header(void) {
    memcpy_P(&hdr, (void *)__image_hdr, sizeof(image_hdr_t));
    return &hdr;
}

int image_validate(const image_hdr_t *hdr) {
    uint16_t image_addr = sizeof(image_hdr_t);
    uint16_t image_size = hdr->image_size;

    if (hdr->image_magic != IMAGE_MAGIC) {
        return IMAGE_INVALID_MAGIC;
    }

    uint32_t crc = ~0x0;
    uint8_t data = 0x00;
    for (; image_size > 0; image_size--, image_addr++) {
        data = pgm_read_byte(image_addr);
        crc32_step(data, &crc);
    }
    crc = ~crc;

    if (crc != hdr->crc) {
        return IMAGE_INVALID_CRC;
    }

    return IMAGE_VALID;
}
