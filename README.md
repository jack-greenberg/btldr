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

- [ ] Update `can_isp_commands` to use `boot_page_fill` instead of `temp_buffer`
- [ ] Update to use builtin crc16
- [ ] Client
- [ ] Reset handling

## Notes

Page 80:

> Note: If Interrupt Vectors are placed in the Boot Loader section and Boot
> Lock bit BLB02 is programmed, interrupts are disabled while executing from
> the Application section. If Interrupt Vectors are placed in the Application
> section and Boot Lock bit BLB12 is programmed, interrupts are disabled while
> executing from the Boot Loader section.

We need to decide some things: on reset, should the bootloader execute first?
Or do we go to the application? Either one is ok, but if we go to the
bootloader first, we can just check to see why there was a reset, and if it
wasn't a bootloader request, we jump to the application.

Do we need to disable interrupts? Where should the interrupts be located?

Would be nice to have a `#define LED1 PB0` pattern so that we can always refer
to `LED1` and use `#ifdefs` to do things like blink an LED if there is an
error, or set one as the error LED.

It seems like we could try and figure out a way to write an arbitrary amount of
data by filling the page buf and checking to see if it is full, and then
writing, but that might be more complicated. In theory, we are writing
contiguous sections of memory, so we shouldn't run into too many issues, but it
would be worth considering the things that could go wrong. The alternative is
doing a Read-Modify-Write each time we want to write data. So we read the whole
page to the temp buffer, modify the buffer, and then write it back.
