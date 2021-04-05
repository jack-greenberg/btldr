#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <inttypes.h>

void flash_program_page(uint32_t page_addr, uint8_t *data) {
    uint16_t i;
    uint8_t sreg = SREG;
    cli();

    boot_page_erase(page_addr);
    boot_spm_busy_wait();

    for (i = 0; i < SPM_PAGESIZE; i += 2) {
        uint16_t d = *data++;
        d <<= 8;
        d &= *data++;
        boot_page_fill(page_addr + i, d);
    }

    boot_page_write(page_addr);
    boot_spm_busy_wait();

    SREG = sreg;
}
