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
#include <stdbool.h>
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
extern uint32_t hello_sequence;
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

	free(init_hello_args_ptr);

	return NULL;
}


void *hello_back_handler(void *hello_back_args_ptr)
{
	struct sockaddr_ll socket_address;
	struct ethhdr *ethhdr_ptr;
	unsigned char eth_frame[HELLO_MAX_PACKET];
	unsigned int eth_frame_len;
	unsigned char if_macaddr[6];
	unsigned char hello_identity[HELLO_IDENTITY_LEN];
	hello_hdr_t hello_back_hdr;
	hello_hdr_t *hello_hdr_ptr;
	bool is_loop;
	int i;

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

	//Get hello identity
	hello_identity_get(hello_identity);

	hello_hdr_ptr = (hello_hdr_t *)(((hello_thread_args_t *)hello_back_args_ptr)->hello_recvd_buff + sizeof(struct ethhdr));

	//Return if loop packet detected
	is_loop = true;
	for (i = 0; i < 6; i++) {
		if ((hello_hdr_ptr->hello_src)[i] != hello_identity[i]) {
			is_loop = false;
			break;
		}
	}
	if (is_loop) {
		free(hello_back_args_ptr);
		return NULL;
	}

	//Fill ethernet header
	ethhdr_ptr = (struct ethhdr *)eth_frame; 
	memcpy(ethhdr_ptr->h_dest, hello_mac_addr, 6);
	memcpy(ethhdr_ptr->h_source, if_macaddr, 6);
	ethhdr_ptr->h_proto = htons(HELLO_MSG_ETH_TYPE);
	eth_frame_len += sizeof(struct ethhdr);

	//Fill hello header
	hello_back_hdr.hello_stage = HELLO_STAGE_II;
	memcpy(hello_back_hdr.hello_src, hello_identity_get(if_macaddr), HELLO_IDENTITY_LEN);
	memcpy(eth_frame + eth_frame_len, &hello_back_hdr, sizeof(hello_hdr_t));
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


void *hello_flood_handler(void *flood_hello_args_ptr)
{
	struct sockaddr_ll socket_address;
	struct ethhdr *ethhdr_ptr;
	unsigned char hello_identity[HELLO_IDENTITY_LEN];
	unsigned char eth_frame[HELLO_MAX_PACKET];
	unsigned int eth_frame_len;
	unsigned char if_macaddr[6];
	hello_hdr_t hello_flood_hdr;
	hello_hdr_t *hello_hdr_ptr;
	bool is_loop;
	int i;


	pthread_detach(pthread_self());
	eth_frame_len = 0;

//TODO: Set up the (flood) outbound ports
// use hello port

	//Get source MAC address
	if (((hello_thread_args_t *)flood_hello_args_ptr)->hello_extra == NULL) {
		getmacaddr(hello_if, if_macaddr);
	} else {
		memcpy(if_macaddr, ((hello_thread_args_t *)flood_hello_args_ptr)->hello_extra, 6);
	}

	//Get hello identity
	hello_identity_get(hello_identity);

	hello_hdr_ptr = (hello_hdr_t *)(((hello_thread_args_t *)flood_hello_args_ptr)->hello_recvd_buff + sizeof(struct ethhdr));

	//Return if loop packet detected
	is_loop = true;
	for (i = 0; i < 6; i++) {
		if ((hello_hdr_ptr->hello_src)[i] != hello_identity[i]) {
			is_loop = false;
			break;
		}
	}
	if (is_loop) {
		free(flood_hello_args_ptr);
		return NULL;
	}

	//Fill ethernet header
	ethhdr_ptr = (struct ethhdr *)eth_frame; 
	memcpy(ethhdr_ptr->h_dest, hello_mac_addr, 6);
	memcpy(ethhdr_ptr->h_source, if_macaddr, 6);
	ethhdr_ptr->h_proto = htons(HELLO_MSG_ETH_TYPE);
	eth_frame_len += sizeof(struct ethhdr);

	//Fill hello header
	hello_flood_hdr.hello_stage = HELLO_STAGE_III;
	hello_flood_hdr.hello_sequence = htonl(hello_sequence);
	hello_flood_hdr.hello_ngbr_bits = *(((hello_thread_args_t *)flood_hello_args_ptr)->hello_ngbr_bits);
	memcpy(hello_flood_hdr.hello_src, hello_identity_get(if_macaddr), HELLO_IDENTITY_LEN);
	memcpy(eth_frame + eth_frame_len, &hello_flood_hdr, sizeof(hello_hdr_t));
	eth_frame_len += sizeof(hello_hdr_t);

	//Fill hello payload
	memcpy(eth_frame + eth_frame_len, ((hello_thread_args_t *)flood_hello_args_ptr)->hello_payload, sizeof(hello_payload_t));
	eth_frame_len += sizeof(hello_payload_t);

	//Fill socket_address
	socket_address.sll_ifindex = getifidx(hello_if);
	socket_address.sll_halen = ETH_ALEN;

	if (sendto(hello_send_raw_socket, eth_frame, eth_frame_len, 0, (struct sockaddr *)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
		printf("Sendto error, failed to send packets\n");
	}

	hello_sequence += 1;
	free(flood_hello_args_ptr);

	return NULL;
}


void hello_back(pthread_t *handler_pid, hello_thread_args_t *hello_thread_universal_args_ptr)
{
	hello_thread_args_t *hello_back_args_ptr;
	hello_back_args_ptr = malloc(sizeof(hello_thread_args_t));
	memcpy(hello_back_args_ptr, hello_thread_universal_args_ptr, sizeof(hello_thread_args_t));
	pthread_create(handler_pid, NULL, &hello_back_handler,  hello_back_args_ptr);
}


void hello_update_neighbor(hello_thread_args_t * hello_args_ptr)
{
	hello_hdr_t *hello_hdr_ptr;
	*(hello_args_ptr->hello_ngbr_bits) |= (1 << hello_args_ptr->hello_port);
	hello_hdr_ptr = (hello_hdr_t *)(hello_args_ptr->hello_recvd_buff + sizeof(struct ethhdr));
	memcpy(hello_args_ptr->hello_payload + (HELLO_IDENTITY_LEN) * hello_args_ptr->hello_port, hello_hdr_ptr->hello_src, HELLO_IDENTITY_LEN);
	return;
}


unsigned char *hello_identity_get(unsigned char *buffer)
{
	return getmacaddr(hello_if, buffer);
}

