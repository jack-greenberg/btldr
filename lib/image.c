#include "image.h"

#include <stdbool.h>
#include <stdint.h>

#include "crc32.h"

extern int __data_start;
extern int __data_end;

image_hdr_t *image_get_header(void) {
    image_hdr_t *hdr = (image_hdr_t *)&__data_start;

    if (hdr && (hdr->image_magic == IMAGE_MAGIC)) {
        return hdr;
    } else {
        return NULL;
    }
}

bool image_validate(image_hdr_t *hdr) {
    if (hdr == NULL) {
        return false;
    }

    void *image_addr = &__data_start;
    image_addr += sizeof(image_hdr_t);
    uint32_t length = hdr->image_size;

    if (hdr->image_magic != IMAGE_MAGIC) {
        return false;
    }

    uint32_t crc;
    crc32(image_addr, (size_t)length, &crc);

    if (crc != hdr->crc) {
        return false;
    }

    return true;
}
