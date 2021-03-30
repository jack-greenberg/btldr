#include <stdint.h>

/*
 * Flash driver
 *
 * From the datasheet 27.8
 *
 * The program memory is updated in a page by page fashion.
 * Before programming a page with the data stored in the temporary
 * page buffer, the page must be erased. The temporary page buffer
 * is filled one word at a time using SPM and the buffer can be
 * filled either before the Page Erase command or between a Page
 * Erase and a Page Write operation
 */

/*
 * Load word into temporary buffer
 *
 * @param [in] addr: Address to write
 * @param [in] word: Word data to wrie
 *
 * Datasheet 27.8.2:
 *   To write an instruction word, set up the address in the
 *   Z-pointer and data in [R1:R0], write “0x00000001” to
 *   SPMCSR and execute SPM within four clock cycles after
 *   writing SPMCSR. The content of PCWORD ([Z5:Z1]) in the
 *   Z-register is used to address the data in the temporary buffer.
 *
 */
void flash_fill_temp_buffer(uint16_t addr, uint16_t word);

/*
 * Write a flash page with the temporary buffer content
 *
 * @param [in] addr: Address to write
 *
 * Datasheet 27.8.3:
 *   To execute Page Write, set up the address in the Z-pointer,
 *   write “0x0000101” to SPMCSR and execute SPM within four
 *   clock cycles after writing SPMCSR. The data in R1 and R0
 *   is ignored. The page address must be written to PCPAGE
 *   ([Z5:Z1]). Other bits in the Z-pointer must be written to
 *   zero during this operation.
 */
void flash_page_write(uint16_t addr);

/*
 * Erase a flash page
 *
 * @param [in] addr: Address to erase
 *
 * Datasheet 27.8.1:
 *   To execute Page Erase, set up the address in the Z-pointer,
 *   write “0x0000011” to SPMCSR and execute SPM within four
 *   clock cycles after writing SPMCSR.The data in R1 and R0 is
 *   ignored. The page address must be written to PCPAGE in
 *   the Z-register.
 */
void flash_page_erase(uint16_t addr);
