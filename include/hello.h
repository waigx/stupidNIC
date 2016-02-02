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


#ifndef __HELLO_H
#define __HELLO_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>


#define HELLO_MSG_ETH_TYPE				0x07FF
#define HELLO_MAX_PACKET				  1500 
#define HELLO_IF_NAME_LEN				    32

#define HELLO_MAX_NEIGHBOR				   0x4
#define HELLO_IDENTITY_LEN				   0x6

#define HELLO_INIT_INTERVAL				     5
#define HELLO_FLOOD_WAIT				     1

#define HELLO_DFT_IF					"eth0"
#define HELLO_DFT_MAC	  {0xff, 0xff, 0xff, 0xff, 0xff, 0xef}


typedef enum hello_stage {
	HELLO_STAGE_I,  
        HELLO_STAGE_II, 
        HELLO_STAGE_III,
} hello_stage_t;


typedef struct hello_hdr {
	unsigned char			hello_src[HELLO_IDENTITY_LEN];
	hello_stage_t			hello_stage;
	uint32_t			hello_sequence;
	unsigned char			hello_ngbr_bits;
	unsigned char			hello_outbound_port;
} hello_hdr_t;


typedef struct hello_payload {
	unsigned char			hello_payload[HELLO_MAX_NEIGHBOR * HELLO_IDENTITY_LEN];
	unsigned char			hello_payload_extra[HELLO_MAX_NEIGHBOR];
} hello_payload_t;


typedef struct hello_thread_args {
	unsigned char *			hello_recvd_buff;
	unsigned char *			hello_ngbr_bits;
	unsigned char *			hello_payload;
	unsigned char *			hello_extra;
	unsigned char			hello_inbound_port;
} hello_thread_args_t;


int hello_send_raw_socket;
char hello_if[HELLO_IF_NAME_LEN];
uint32_t hello_sequence;
unsigned char hello_mac_addr[6];

void *hello_init_handler(void *);
void *hello_flood_handler(void *);
void *hello_back_handler(void *);

bool hello_is_loop(hello_thread_args_t *);

void hello_back(pthread_t *, hello_thread_args_t *);
void hello_update_neighbor(hello_thread_args_t *);

unsigned char *hello_identity_get(unsigned char *);

#endif
