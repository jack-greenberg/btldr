/*
 * CRC32 from http://home.thep.lu.se/~bjorn/crc/
 */

#include <avr/io.h>
#include <inttypes.h>
#include <stdlib.h>

void crc32(const void *data, size_t n_bytes, uint32_t *crc);
