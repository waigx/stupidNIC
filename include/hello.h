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

#include <stdint.h>


#define HELLO_MSG_ETH_TYPE			0x07FF

#define HELLO_MAX_NEIGHBOR			   0x4
#define HELLO_IDENTITY_LEN			   0x6


#define HELLO_INIT_INTERVAL			     5
#define HELLO_FLOOD_WAIT			     1


typedef enum hello_stage {
	HELLO_STAGE_I,  
        HELLO_STAGE_II, 
        HELLO_STAGE_III,
} hello_stage_t;


typedef struct hello_hdr {
	unsigned char			hello_src[HELLO_IDENTITY_LEN];
	hello_stage_t			hello_stage;
	unsigned int			hello_sequence;
	unsigned char			hello_ngbr_bits;
} hello_hdr_t;


typedef struct hello_payload {
	unsigned char			hello_payload[HELLO_MAX_NEIGHBOR * HELLO_IDENTITY_LEN];
} hello_payload_t;


int hello_send_raw_socket;

void hello_init_handler(void *);
void hello_flood_handler(void *);
void hello_back_handler(void *);

void hello_back(unsigned char *);
void hello_update_neighbor(unsigned char *);

char * hello_identity_get(char *);

#endif
