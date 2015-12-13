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


#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <sys/socket.h>

#include <hello.h>


#define PACKET_SIZE_MAX 65536


//static unsigned char ngbr_bits;
//static unsigned char ngbr_addr[HELLO_MAX_NEIGHBOR * HELLO_IDENTITY_LEN];
extern int hello_send_raw_socket;

void dump_packet(unsigned char *, int);
void packet_processor(unsigned char *);

void alarm_hello_init(int);
void alarm_hello_flood(int);


static struct sigaction hello_init = {
	.sa_handler = alarm_hello_init,
	.sa_flags = SA_RESTART,
};

static struct sigaction hello_flood = {
	.sa_handler = alarm_hello_flood,
	.sa_flags = SA_RESTART,
};


int main(int argc, char *argv[], char *envp[])
{
	int hello_recv_raw_socket;
	int buffer_size;
	int sockaddr_size;
	struct sockaddr socket_address;
	struct ethhdr *ethhdr_ptr;
	unsigned char *buffer;

	hello_send_raw_socket = socket( PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)) ;
	if(hello_send_raw_socket < 0){
		printf("Send raw socket creation error.\n");
		return 1;
	}

	hello_recv_raw_socket = socket( PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)) ;
	if(hello_recv_raw_socket < 0){
		printf("Recv raw socket creation error.\n");
		return 1;
	}

	buffer = (unsigned char *) malloc(PACKET_SIZE_MAX);

	sigaction(SIGALRM, &hello_init, NULL);
	alarm(HELLO_INIT_INTERVAL);

	while(true){
		sockaddr_size = sizeof(socket_address);
		buffer_size = recvfrom(hello_recv_raw_socket, buffer, PACKET_SIZE_MAX, 0, &socket_address, (socklen_t*)&sockaddr_size);
		if(buffer_size<0 ){
			printf("Recvfrom error, failed to get packets\n");
			return 1;
		}
//		dump_packet(buffer, buffer_size);

		ethhdr_ptr = (struct ethhdr *)buffer;
		if (ethhdr_ptr->h_proto == HELLO_MSG_ETH_TYPE){
			packet_processor(buffer);
		}
	}
	close(hello_recv_raw_socket);
	free(buffer);
	return 0;
}


void dump_packet(unsigned char *buffer, int buffer_size)
{
	int i;

	for (i = 0; i < buffer_size; i++){
		printf("%x", buffer[i]);
	}

	printf("\n");
	return;
}


void packet_processor(unsigned char *buffer)
{
	hello_hdr_t *hello_hdr_ptr;

	hello_hdr_ptr = (hello_hdr_t *)(buffer + sizeof(struct ethhdr));
	switch (hello_hdr_ptr->hello_stage){
		case HELLO_STAGE_I:
			hello_back(buffer);
			break;

		case HELLO_STAGE_II:
			hello_update_neighbor(buffer);
			break;

		case HELLO_STAGE_III:
// TODO:
//			update_topo(buffer);
			break;

		default:
			break;
	}

	return;
}


void alarm_hello_init(int signo)
{
	sigaction(SIGALRM, &hello_flood, &hello_init);
	alarm(HELLO_FLOOD_WAIT);
}


void alarm_hello_flood(int signo)
{
	sigaction(SIGALRM, &hello_init, &hello_flood);
	alarm(HELLO_INIT_INTERVAL - HELLO_FLOOD_WAIT);
}
