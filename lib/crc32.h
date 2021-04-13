/*
 * CRC32 from http://home.thep.lu.se/~bjorn/crc/
 */
#pragma once
#include <avr/io.h>
#include <inttypes.h>
#include <stdlib.h>

/*
 * Generate CRC32 digest of data
 */
void crc32(uint8_t *data, size_t n_bytes, uint32_t *crc);

/*
 * One step of CRC32 digest of data
 */
void crc32_step(uint8_t data, uint32_t *crc);
