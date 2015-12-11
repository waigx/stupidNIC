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
	uint8_t					hello_stage;
	uint					hello_sequence;
	unsigned char			hello_ngbr_bits;
} hello_hdr;


typedef struct hello_payload {
	unsigned char			hello_payload[HELLO_MAX_NEIGHBOR * HELLO_IDENTITY_LEN];
} hello_payload;


#endif
