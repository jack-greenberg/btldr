---
author: Jack Greenberg
---

# BTLDR

CANbus bootloader for AVR

Presented by Jack Greenberg

---

## Agenda

1. Background
2. Architecture
3. App Protocol
4. CLI Client
5. Questions

---

# Background

> What is a bootloader?

An application (has a `main` function) that runs on the microcontroller
```
```
Responsible for updating and validating the main application
```
```
Runs when the device resets (or starts up)

---

# Architecture

---

```
                                                Flash Memory
                                                ┌──────────────────┐0x0000
                                                │                  │
                                                │                  │
                                                │                  │
                                                │                  │
                                                │        App       │
                                                │                  │
                                                │                  │
                                                │                  │
                                                │                  │
                                                ├──────────────────┤0x3000
                                                │                  │
                                                │    Bootloader    │
                                                │                  │
                                                └──────────────────┘

```

* ATmega16m1 flash is divided into two sections: __bootloader__ and __flash__
* Bootloader is an application is responsible for updating and running the main application

---

# Architecture

## Bootloader
