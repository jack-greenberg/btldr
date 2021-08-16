# BTLDR

**CAN Bootloader for ATMega16m1**

This repository holds the code for the CAN bootloader for the ATMega16m1
microcontroller.

The code lives in `/src/`.  `main.c` contains a "dummy" image for the
microcontroller: a simple blinky program.  `btldr.c` is the bootloader image.
`config.h` should be used for any configuration variables.

`/old/` contains the bootloader we are basing ours off of, which comes from
Atmel.

`/lib/` contains the libraries that we are writing. These are the libraries:

* __CAN_drv__: CAN driver (interfaces with the registers and such) __CAN_lib__:
* Provides higher-level interface for CAN, like `send` and `get_message`
* __CAN_isp__: Defines CAN messages that are used to program the target board
* __ISP__: In-system-programming library for programming __Flash_drv__: Drivers
* for interfacing with flash memory __Flash_lib__: Library (higher level) for
* doing flashing

## Contributing

Please help us!  Below you'll see the __ToDo__ section.  To make a
contribution, make your changes locally and submit a PR.  As a part of your PR,
change the `[ ]` in your todo item to a `[x]` to indicate that it has been
done.  Please do one `[x]` per PR.

## ToDo

- [x] Update `can_isp_commands` to use `boot_page_fill` instead of `temp_buffer`
- [ ] Update to use builtin crc16
- [ ] Client
- [ ] Reset handling (how to have ECUs reset themselves--just jump to boot
  start?)
- [ ] How to handle a common CAN command handling for all ECUs (to listen for
  reset)

## Notes

Page 80:

> Note: If Interrupt Vectors are placed in the Boot Loader section and Boot
> Lock bit BLB02 is programmed, interrupts are disabled while executing from
> the Application section. If Interrupt Vectors are placed in the Application
> section and Boot Lock bit BLB12 is programmed, interrupts are disabled while
> executing from the Boot Loader section.

Do we need to disable interrupts? Where should the interrupts be located?

Would be nice to have a `#define LED1 PB0` pattern so that we can always refer
to `LED1` and use `#ifdefs` to do things like blink an LED if there is an
error, or set one as the error LED.

## Patch Header

* Compile-time
  * Image size
  * CRC
* Flash-time
  * Timestamp
* Both
  * Verify image magic
