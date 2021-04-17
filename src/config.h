#pragma once

/*
 * Bootloader version is defined as 8-bit number
 * MMMM.mmmm
 *
 * M := Major Revision
 * m := Minor revision
 *
 * CURRENT VERSION: 0.1
 */
#define BOOTLOADER_VERSION_MAJ (0)
#define BOOTLOADER_VERSION_MIN (1)

/*
 * ECU Node ID. Eventually this should be defined in a central place like a
 * JSON file, but for now we will set a default here.
 *
 * Default node ID will be 0xFF.
 *
 * TODO: Develop a standard for ECU management
 */
#ifndef NODE_ID
#define NODE_ID (0xFF)
#endif
