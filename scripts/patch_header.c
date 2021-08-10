/*
 * Run right before flashing
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "crc32.h"
#include "image.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: patch_header BINARY\n");
        return 1;
    }

    size_t image_hdr_size = sizeof(image_hdr_t);
#ifdef DEBUG
    printf("Image header size: %ld\n", image_hdr_size);
#endif

    char* file = argv[1];
    FILE* fp = fopen(file, "r+b");

    if (fp == NULL) {
        fprintf(stderr, "File not found: %s\n", file);
        return 1;
    }

#ifdef DEBUG
    printf("Patching %s\n", file);
#endif

    image_hdr_t image_hdr;
    fread(&image_hdr, image_hdr_size, 1, fp);

#ifdef DEBUG
    printf("Found data: {\n");
    printf("    .image_magic = %x\n", image_hdr.image_magic);
    printf("    .crc = %x\n", image_hdr.crc);
    printf("    .image_size = %d\n", image_hdr.image_size);
    printf("    .flash_timestamp = %ld\n", image_hdr.flash_timestamp);
    printf("    .reserved = %x\n", image_hdr.reserved);
    printf("    .git_sha = %s\n", image_hdr.git_sha);
    printf("}\n");
#endif

    if (image_hdr.image_magic != IMAGE_MAGIC) {
        fprintf(stderr, "Incorrect image magic: %x\n", image_hdr.image_magic);
        fclose(fp);
        return 1;
    }

    // Timestamp
    time_t timestamp = time(NULL);
    image_hdr.flash_timestamp = (uint64_t)timestamp;

#ifdef DEBUG
    printf("Setting timestamp to %ld\n", image_hdr.flash_timestamp);
#endif

    // Image size
    fseek(fp, 0, SEEK_END);
    size_t image_size_total = ftell(fp);
    uint16_t image_size = image_size_total - image_hdr_size;

#ifdef DEBUG
    printf("Total image size: %ld\nImage size w/o header: %d\n",
           image_size_total, image_size);
#endif

    image_hdr.image_size = image_size;

    // CRC
    fseek(fp, image_hdr_size, SEEK_SET);

    uint8_t* image = (uint8_t*)malloc(image_size);
    fread(image, image_size, 1, fp);

    uint32_t crc;
    crc32(image, image_size, &crc);
    image_hdr.crc = crc;

#ifdef DEBUG
    printf("Calculated CRC32: 0x%x\n", image_hdr.crc);
#endif

    free(image);

    // Write image_hdr back to binary file
    fseek(fp, 0, SEEK_SET);
    if (fwrite(&image_hdr, image_hdr_size, 1, fp) == 0) {
        fprintf(stderr, "Write failed.\n");
    }

    printf("Image data written.\n");

    fclose(fp);
    return 0;
}
