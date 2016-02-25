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
#include <stdint.h>
#include <stdlib.h>

#include <hello.h>
#include <nettopo.h>
#include <nettopoutils.h>


extern nettopo_graph_t nettopo_graph;

void _feed_hello_portocal(unsigned char *name, unsigned char *neighbors, unsigned char *outbound_ports)
{
	int i, j;
	printf("Input Nodes Name: ");
	scanf("%s", name);
	printf("Input Nodes Neighbors: ");
	scanf("%s %s %s %s", neighbors, neighbors + HELLO_IDENTITY_LEN, neighbors + HELLO_IDENTITY_LEN * 2, neighbors + HELLO_IDENTITY_LEN * 3);
	for (i = 0; i < HELLO_MAX_NEIGHBOR; i++) {
		if (*(neighbors + i * HELLO_IDENTITY_LEN) == '.') {
			for (j = 0; j < HELLO_IDENTITY_LEN; j++)
				*(neighbors + i * HELLO_IDENTITY_LEN + j) = 0;
		}

	}
	printf("Input Neighbors' Ports: ");
	scanf("%hhu %hhu %hhu %hhu", outbound_ports, outbound_ports + 1, outbound_ports + 2, outbound_ports + 3);
}


int main(int argc, char * argv[], char * envp[])
{
	char in;
	unsigned char buffer[HELLO_IDENTITY_LEN * 6] = {0};
	unsigned char * name, * neighbors, * outbound_ports;

	name = buffer;
	neighbors = buffer + HELLO_IDENTITY_LEN;
	outbound_ports = buffer + HELLO_IDENTITY_LEN * 5;

	in = 0;
	while (in != 'q') {
		printf("Dump(d), Update(u), Calculate(c), Quite(q): ");
		in = getchar();
		switch (in) {
		case 'd':
			nttutil_dump_graph(&nettopo_graph);
			printf("\n");
			break;
		case 'c':
			nettopo_run_dijkstra(&nettopo_graph, nettopo_graph.topo_nodes[0]);
			break;
		case 'q':
			return 0;
			break;
		case 'u':
			_feed_hello_portocal(name, neighbors, outbound_ports);
			nettopo_update_graph(name, neighbors, outbound_ports);	
			break;
		default:
			break;
		}
		getchar();
	}
	return 0;
}
