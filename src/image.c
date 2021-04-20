#include "image.h"

#include <stdint.h>

#include "crc32.h"

extern int __image_hdr;

const image_hdr_t *image_get_header(void) {
    const image_hdr_t *hdr = (image_hdr_t *)&__image_hdr;

    if (hdr && (hdr->image_magic == IMAGE_MAGIC)) {
        return hdr;
    } else {
        return NULL;
    }
}

int image_validate(const image_hdr_t *hdr) {
    // const void *image_addr = &__data_start;
    // image_addr += sizeof(image_hdr_t);
    // uint32_t length = hdr->image_size;
    // uint32_t crc;
    // crc32(image_addr, (size_t)length, &crc);

    return 1;
}
