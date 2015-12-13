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

#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <sys/socket.h>

#include <hello.h>


extern int hello_send_raw_socket;

void init_hello_handler(void *init_hello_args_ptr)
{

//	if (init_hello_args_ptr == NULL) {
//	}
//
//	hello_hdr_t init_hello_hdr = {
//		.hello_stage = HELLO_STAGE_I,
//	};
//	pthread_detach(pthread_self());
}


void flood_hello_handler(void *flood_hello_args_ptr)
{
}


void hello_back_handler(void *hello_back_args_ptr)
{
}


void hello_back(unsigned char *buffer)
{
	
}


void hello_update_neighbor(unsigned char *buffer)
{
	
}


char * hello_identity_get(char *buffer)
{
	return NULL;
}
