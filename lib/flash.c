#include "flash.h"

#include <avr/boot.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>

void flash_write(uint8_t *data, uint16_t address) {
    uint8_t sreg = SREG;
    dword_t d;

    boot_spm_busy_wait();
    for (uint8_t i = 0; i < PAGESIZE_B; i += 2) {
        d.byte[0] = *data++;
        d.byte[1] = *data++;

        boot_page_fill(address + i, d.word);
    }

    boot_page_erase(address);  // Must erase the page before writing
    boot_page_write(address);  // Write the memory stored in the temp buffer
    boot_spm_busy_wait();

    SREG = sreg;
}

void flash_read(uint16_t address, uint8_t *data, uint8_t size) {
    for (uint8_t i = 0; i < size; i++) {
        *data = pgm_read_byte(address + i);
        data++;
    }
}
