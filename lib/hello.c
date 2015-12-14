/*
 *
 *  This file is part of project SmartNIC
 *
 *  Copyright (C) 2015 Yigong Wang <yigwang@cs.stonybrook.edu>
 * 
 *
 *  StupidNIC is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  StupidNIC is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with StupidNIC.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netpacket/packet.h>
#include <sys/socket.h>

#include <hello.h>
#include <ifutils.h>

#include <arpa/inet.h>
#include <sys/ioctl.h>

extern int hello_send_raw_socket;
extern char hello_if[HELLO_IF_NAME_LEN];

void *hello_init_handler(void *init_hello_args_ptr)
{
	struct sockaddr_ll socket_address;
	struct ethhdr *ethhdr_ptr;
	unsigned char eth_frame[HELLO_MAX_PACKET];
	unsigned int eth_frame_len;
	unsigned char if_macaddr[6];
	hello_hdr_t hello_init_hdr;

	pthread_detach(pthread_self());
	eth_frame_len = 0;

//TODO: Set up the (flood) outbound ports
//
	//Get source MAC address
	if (((hello_thread_args_t *)init_hello_args_ptr)->hello_extra == NULL) {
		getmacaddr(hello_if, if_macaddr);
	} else {
		memcpy(if_macaddr, ((hello_thread_args_t *)init_hello_args_ptr)->hello_extra, 6);
	}

	//Fill ethernet header
	ethhdr_ptr = (struct ethhdr *)eth_frame; 
	memcpy(ethhdr_ptr->h_dest, hello_mac_addr, 6);
	memcpy(ethhdr_ptr->h_source, if_macaddr, 6);
	ethhdr_ptr->h_proto = htons(HELLO_MSG_ETH_TYPE);
	eth_frame_len += sizeof(struct ethhdr);

	//Fill hello header
	hello_init_hdr.hello_stage = HELLO_STAGE_I;
	memcpy(hello_init_hdr.hello_src, hello_identity_get(if_macaddr), HELLO_IDENTITY_LEN);
	memcpy(eth_frame + eth_frame_len, &hello_init_hdr, sizeof(hello_hdr_t));
	eth_frame_len += sizeof(hello_hdr_t);

	//Fill socket_address
	socket_address.sll_ifindex = getifidx(hello_if);
	socket_address.sll_halen = ETH_ALEN;

	if (sendto(hello_send_raw_socket, eth_frame, eth_frame_len, 0, (struct sockaddr *)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
		printf("Sendto error, failed to send packets\n");
	}
	return NULL;
}


void *hello_flood_handler(void *flood_hello_args_ptr)
{
	return NULL;
}


void *hello_back_handler(void *hello_back_args_ptr)
{
	struct sockaddr_ll socket_address;
	struct ethhdr *ethhdr_ptr;
	unsigned char eth_frame[HELLO_MAX_PACKET];
	unsigned int eth_frame_len;
	unsigned char if_macaddr[6];
	hello_hdr_t hello_init_hdr;

	pthread_detach(pthread_self());
	eth_frame_len = 0;

//TODO: Set up the outbound port (equal to imcomming port)
// use hello port

	//Get source MAC address
	if (((hello_thread_args_t *)hello_back_args_ptr)->hello_extra == NULL) {
		getmacaddr(hello_if, if_macaddr);
	} else {
		memcpy(if_macaddr, ((hello_thread_args_t *)hello_back_args_ptr)->hello_extra, 6);
	}


	//Fill ethernet header
	ethhdr_ptr = (struct ethhdr *)eth_frame; 
	memcpy(ethhdr_ptr->h_dest, hello_mac_addr, 6);
	memcpy(ethhdr_ptr->h_source, if_macaddr, 6);
	ethhdr_ptr->h_proto = htons(HELLO_MSG_ETH_TYPE);
	eth_frame_len += sizeof(struct ethhdr);

	//Fill hello header
	hello_init_hdr.hello_stage = HELLO_STAGE_II;
	memcpy(hello_init_hdr.hello_src, hello_identity_get(if_macaddr), HELLO_IDENTITY_LEN);
	memcpy(eth_frame + eth_frame_len, &hello_init_hdr, sizeof(hello_hdr_t));
	eth_frame_len += sizeof(hello_hdr_t);

	//Fill socket_address
	socket_address.sll_ifindex = getifidx(hello_if);
	socket_address.sll_halen = ETH_ALEN;

	if (sendto(hello_send_raw_socket, eth_frame, eth_frame_len, 0, (struct sockaddr *)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
		printf("Sendto error, failed to send packets\n");
	}

	free(hello_back_args_ptr);

	return NULL;
}


void hello_back(pthread_t *handler_pid, unsigned char hello_port)
{
	hello_thread_args_t *hello_back_args_ptr;
	hello_back_args_ptr = malloc(sizeof(hello_thread_args_t));
	hello_back_args_ptr->hello_port = hello_port;
	pthread_create(handler_pid, NULL, &hello_flood_handler,  &hello_back_args_ptr);
}


void hello_update_neighbor(unsigned char *buffer)
{
	
}


unsigned char *hello_identity_get(unsigned char *buffer)
{
	return getmacaddr(hello_if, buffer);
}
