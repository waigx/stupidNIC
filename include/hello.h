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

#include <unistd.h>


#define HELLO_MSG_ETH_TYPE			0x07FF

#define HELLO_STAGE_I				   0x1
#define HELLO_STAGE_II				   0x2
#define HELLO_STAGE_III				   0x3

#define HELLO_MAX_NEIGHBOR			   0x4
#define HELLO_IDENTITY_LEN			   0x6


typedef struct hello_hdr {
	uint8_t				hello_stage;
	uint				hello_sequence;
	unsigned char			hello_ngbr_bits;
} hello_hdr;


typedef struct hello_payload {
	unsigned char			hello_payload[HELLO_MAX_NEIGHBOR * HELLO_IDENTITY_LEN];
} hello_payload;


#endif
