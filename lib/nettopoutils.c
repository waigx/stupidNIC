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
#include <stdlib.h>
#include <hello.h>
#include <nettopo.h>


void _dump_node(nettopo_node_t *node)
{
	int i;
	printf("%s\t------------\t|\n", node->topo_idtt);
	for (i = 0; i < HELLO_MAX_NEIGHBOR; i++) {
		if (node->topo_ngbr[i] == NULL) {
			printf("\t\t\t|(%hhu) --\n", i);
			continue;
		} else {
			printf("\t\t\t|(%hhu) --- %s\n", i, node->topo_ngbr[i]->topo_idtt);
		}
	}
}


void nttutil_dump_graph(nettopo_graph_t *graph)
{
	int i;
	printf("Nodes Number: %llu\n", (long long unsigned int)(graph->topo_nodes_number));
	for (i = 0 ; i < graph->topo_nodes_number; i++) {
		_dump_node(graph->topo_nodes[i]);
		printf("\n");
	}
}

