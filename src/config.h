#pragma once

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
