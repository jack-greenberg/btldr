#pragma once
#include <stdint.h>

#include <net/if.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

struct CanClient {
    int s; // Socket
    int mtu;
	struct sockaddr_can addr;
	struct can_frame frame;
	struct ifreq ifr;
};

/*
 * Initializes the CAN client and sockets
 */
int init_can_client(uint16_t can_filter_id, uint16_t can_filter_mask);

/*
 * Closes socket
 */
void can_client_destroy(void);

/*
 * Sends a single CAN message
 */
int can_send(uint16_t id, uint8_t* data, uint8_t dlc);

/*
 * Set timeout to positive milliseconds
 * Set timeout negative to be infinite
 * Set timeout to zero to instantly return
 */
int can_receive(uint8_t* can_id, uint8_t* can_dlc, uint8_t* data, int timeout);
