/*
 * Author: Jack Greenberg <j@jackgreenberg.co>
 * Copyright 2021
 */
#include "can.h"

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "log.h"

can_client_t* can_client_create(char* if_device) {
    can_client_t* client = calloc(1, sizeof(can_client_t));
    if (client == NULL) {
        return NULL;
    }

    // Open a socket to write to
    if ((client->s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        log_error("Couldn't open socket");
        return NULL;
    }
    log_info("Socket opened successfully");
    client->addr.can_family = AF_CAN;

    // Set up the device and connect the socket to it
    strncpy(client->ifr.ifr_name, if_device, IFNAMSIZ - 1);
    client->ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    if (ioctl(client->s, SIOCGIFINDEX, &client->ifr) < 0) {
        log_error("Failed to find device %s", client->ifr.ifr_name);
        return NULL;
    }
    log_info("Found device %s", client->ifr.ifr_name);
    client->addr.can_ifindex = client->ifr.ifr_ifindex;

    // Set up max transmission unit (mtu)
    if (ioctl(client->s, SIOCGIFMTU, &client->ifr) < 0) {
        log_error("CAN message won't fit in socket");
        return NULL;
    }

    // After the socket is initialized and the device is found, we bind to the
    // socket to enable us to write to it
    if (bind(client->s, (struct sockaddr*)&client->addr, sizeof(client->addr))
        < 0) {
        log_error("Failed to bind to socket");
        return NULL;
    }
    log_info("Successfully binded to socket");

    return client;
}

int can_send(can_client_t* client, uint16_t id, uint8_t* data, uint8_t dlc) {
    // IDs are 11-bits, so enforce that limit
    client->frame.can_id = id & 0x7FF;

    // DLC is at most 8
    if (dlc > 8) {
        dlc = 8;
    }

    client->frame.can_dlc = dlc;
    memcpy(client->frame.data, data, dlc);

    // Write to the socket to send the message
    int nbytes = write(client->s, &client->frame, sizeof(struct can_frame));

    if (nbytes < 0) {
        log_error("Failed to send CAN message with DLC %i and ID 0x%X", dlc,
                  id);
        return 1;
    }

    if (nbytes < dlc) {
        log_warn("CAN frame sent incomplete: dlc=%i", dlc);
    }

    return 0;
}

int can_receive(can_client_t* client, struct can_filter* filter,
                uint16_t* can_id, uint8_t* can_dlc, uint8_t* data,
                int timeout) {
    // Set up socket to receive the CAN message
    setsockopt(client->s, SOL_CAN_RAW, CAN_RAW_FILTER, filter, sizeof(filter));

    // Sets up polling interface
    struct pollfd fds = {
        .fd = client->s,
        .events = POLLIN,
    };

    // Poll for the specified amount of time
    if (poll(&fds, 1, timeout) < 0) {
        log_error("Failed to poll for CAN message");
        return 1;
    }

    // If the event hasn't happened yet, return -1
    if ((fds.revents & POLLIN) == 0) {
        return -1;
    }

    // Poll was successful, so read from the socket
    int nbytes = read(client->s, &client->frame, sizeof(struct can_frame));
    if (nbytes < 0) {
        log_error("Failed to read CAN message");
        return 1;
    }

    // Fill in the appropriate data
    *can_id = client->frame.can_id;
    *can_dlc = client->frame.can_dlc;
    memcpy(data, client->frame.data, client->frame.can_dlc);

    return 0;
}

void can_client_destroy(can_client_t* client) {
    static int i = 0;

    // Will only happen once
    if (i == 0) {
        close(client->s);
        free(client);
        i++;
    }
}
