/*
 * @file atmega16m1.ld
 * @brief Linker script for ATMega16m1
 *
 * @author Jack Greenberg <j@jackgreenberg.co>
 */

MEMORY {
    rom (rx):      ORIGIN = 0x00000000, LENGTH = 16K
    ram (rwx):     ORIGIN = 0x01000000, LENGTH = 1K
    bootrom (rx):  ORIGIN = 0x00003000, LENGTH = 4K
}

SECTIONS {
    .text :
    {
        KEEP(*(.vector*))    
        *(.text*)
    } > rom
    .bss (NOLOAD) :
    {
        *(.bss*)
        *(COMMON)
    } > ram
}
