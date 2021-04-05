# BTLDR

**CAN Bootloader for ATMega16m1**

This repository holds the code for the CAN bootloader for the ATMega16m1 microcontroller.

The code lives in `/src/`.
`main.c` contains a "dummy" image for the microcontroller: a simple blinky program.
`btldr.c` is the bootloader image.
`config.h` should be used for any configuration variables.

`/old/` contains the bootloader we are basing ours off of, which comes from Atmel.

`/lib/` contains the libraries that we are writing. These are the libraries:

* __CAN_drv__: CAN driver (interfaces with the registers and such)
* __CAN_lib__: Provides higher-level interface for CAN, like `send` and `get_message`
* __CAN_isp__: Defines CAN messages that are used to program the target board
* __ISP__: In-system-programming library for programming
* __Flash_drv__: Drivers for interfacing with flash memory
* __Flash_lib__: Library (higher level) for doing flashing

## Contributing

Please help us!
Below you'll see the __ToDo__ section.
To make a contribution, make your changes locally and submit a PR.
As a part of your PR, change the `[ ]` in your todo item to a `[x]` to indicate that it has been done.
Please do one `[x]` per PR.

## ToDo

- [ ] CAN
    - [ ] Driver
    - [ ] Library
    - [ ] ISP command library
- [ ] Flash
    - [ ] Driver
    - [ ] Library
- [ ] ISP Library
- [ ] Blinky (`main.c`)
- [ ] Build system (improving the Makefile)
- [ ] Documentation
- [ ] Main bootloader (`btldr.c`)
    - This is mostly done, and @greenberg-jack is working on it

## Notes

Page 80:

> Note: If Interrupt Vectors are placed in the Boot Loader section and Boot Lock bit BLB02 is
> programmed, interrupts are disabled while executing from the Application section. If
> Interrupt Vectors are placed in the Application section and Boot Lock bit BLB12 is programmed,
> interrupts are disabled while executing from the Boot Loader section.

We need to decide some things: on reset, should the bootloader execute first? Or do we go to the application? Either one is ok, but if we go to the bootloader first, we can just check to see why there was a reset, and if it wasn't a bootloader request, we jump to the application.

Do we need to disable interrupts? Where should the interrupts be located?


