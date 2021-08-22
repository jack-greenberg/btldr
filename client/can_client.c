/*
 * Author: Jack Greenberg <j@jackgreenberg.co>
 * Copyright 2021
 */
#include "can_client.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "log.h"

static struct CanClient client;
extern char* device;

int init_can_client(uint16_t can_filter_id, uint16_t can_filter_mask) {
    // Open a socket to write to
    if ((client.s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        log_error("Couldn't open socket");
        exit(1);
    }
    log_info("Socket opened successfully");
    client.addr.can_family = AF_CAN;

    struct can_filter rfilter[1];
    rfilter[0].can_id = can_filter_id;
    rfilter[0].can_mask = can_filter_mask;
    setsockopt(client.s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    // Set up the device and connect the socket to it
	strncpy(client.ifr.ifr_name, device, IFNAMSIZ - 1);
	client.ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	if (ioctl(client.s, SIOCGIFINDEX, &client.ifr) < 0) {
		log_error("Failed to find device %s", client.ifr.ifr_name);
		return 1;
	}
    log_info("Found device %s", client.ifr.ifr_name);
	client.addr.can_ifindex = client.ifr.ifr_ifindex;

    // Set up max transmission unit (mtu)
    if (ioctl(client.s, SIOCGIFMTU, &client.ifr) < 0) {
        log_error("CAN message won't fit in socket");
        return 1;
    }

    if (bind(client.s, (struct sockaddr *)&client.addr, sizeof(client.addr)) < 0) {
        log_error("Failed to bind to socket");
		return 1;
	}
    log_info("Successfully binded to socket");

    return 0;
}

int can_send(uint16_t id, uint8_t* data, uint8_t dlc) {
    client.frame.can_id = id & 0x7FF;

    // DLC is max 8
    if (dlc > 8) {
        dlc = 8;
    }

    client.frame.can_dlc = dlc;
    memcpy(client.frame.data, data, dlc);

    int nbytes = write(client.s, &client.frame, sizeof(struct can_frame));

    if (nbytes < 0) {
        log_error("Failed to send CAN message with DLC %i and ID 0x%X", dlc, id);
        return 1;
    }

    log_trace("Sent CAN message: id=0x%X, dlc=%i", id, dlc);

    if (nbytes < dlc) {
        log_warn("CAN frame sent incomplete: dlc=%i", dlc);
    }

    return 0;
}

int can_receive(uint8_t* can_id, uint8_t* can_dlc, uint8_t* data) {
    int nbytes = read(client.s, &client.frame, sizeof(struct can_frame));

    if (nbytes < 0) {
        log_error("Failed to read CAN message");
        return 1;
    }

    log_trace("Received CAN message: id=0x%X, dlc=%i", client.frame.can_id, client.frame.can_dlc);

    *can_id = client.frame.can_id;
    *can_dlc = client.frame.can_dlc;
    memcpy(data, client.frame.data, client.frame.can_dlc);

    return 0;
}

void can_client_destroy(void) {
    static int i = 0;

    // Will only happen once
    if (i == 0) {
        close(client.s);
        log_info("Closed socket");
        i++;
    }
}
