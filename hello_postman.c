#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <sys/socket.h>

#include <hello.h>


#define PACKET_SIZE_MAX 65536



void dump_packet(unsigned char *, int);
void packet_processor(unsigned char *);


int main(int argc, char *argv[], char *envp[])
{
	int raw_socket;
	int buffer_size;
	int sockaddr_size;
    struct sockaddr socket_address;
	struct ethhdr *ethhdr_ptr;
	unsigned char *buffer;

	raw_socket = socket( PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)) ;
	buffer = (unsigned char *) malloc(PACKET_SIZE_MAX);

	if(raw_socket < 0){
		printf("Raw socket creation error.\n");
		return 1;
	}

	while(1){
		sockaddr_size = sizeof(socket_address);
		buffer_size = recvfrom(raw_socket, buffer, PACKET_SIZE_MAX, 0, &socket_address, (socklen_t*)&sockaddr_size);
		if(buffer_size<0 ){
			printf("Recvfrom error, failed to get packets\n");
			return 1;
		}

		ethhdr_ptr = (struct ethhdr *)buffer;
		if (ethhdr_ptr->h_proto == HELLO_MSG_ETH_TYPE){
			packet_processor(buffer);
		}
	}
	close(raw_socket);
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
	hello_hdr *hello_hdr_ptr;

	hello_hdr_ptr = (hello_hdr *)(buffer + sizeof(struct ethhdr));
	switch (hello_hdr_ptr->hello_stage){
		case HELLO_STAGE_I:
			hello_back();
			break;

		case HELLO_STAGE_II:
			update_neighbor();
			break;

		case HELLO_STAGE_III:
			update_topo();
			break;

		default:
			break;
	}

	return;
}
