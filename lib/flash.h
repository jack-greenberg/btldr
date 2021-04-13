#pragma once
#include <inttypes.h>

// Page size in bytes
#define PAGESIZE_B (SPM_PAGESIZE)

typedef union {
    uint16_t word;
    uint8_t byte[2];
} dword_t;

/*
 * Writes a page to flash memory
 *
 * @param [in] data     Pointer to data to write
 * @param [in] address  Address to write
 */
void flash_write(uint8_t* data, uint16_t address);

/*
 * Reads a section of flash memory
 *
 * @param [in]  address  Address to write
 * @param [out] data     Pointer to data where data will be read to
 * @param [in]  size     Amount of data
 */
void flash_read(uint16_t address, uint8_t* data, uint8_t size);
