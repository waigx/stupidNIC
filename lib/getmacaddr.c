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


#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>


unsigned char *getmacaddr(const char *if_name, unsigned char *buffer)
{
	int temp_socket;
	struct ifreq ifr;

	if ((temp_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return NULL;
	}

	strcpy(ifr.ifr_name, if_name);
	if (ioctl(temp_socket, SIOCGIFHWADDR, &ifr) < 0) {
		perror("ioctl");
		return NULL;
	}

	memcpy(buffer, ifr.ifr_hwaddr.sa_data, 6);
	close(temp_socket);

	return buffer;
}