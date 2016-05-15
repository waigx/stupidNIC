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


bool _is_same_node_idtt(unsigned char * node_1, unsigned char * node_2);
bool _is_blank_idtt(unsigned char * node_idtt);
nettopo_node_t * _get_node_by_idtt(unsigned char * node_idtt, nettopo_node_t ** node_array, uint64_t node_array_len);
int _get_index_by_node(nettopo_node_t * node, nettopo_node_t ** node_array, uint64_t node_array_len);


void _dump_node(nettopo_node_t *node, nettopo_graph_t * graph, int node_index)
{
	int i;
	int outbound_port;

	if (graph->topo_next_hop[node_index] == NULL)
		printf("(NULL)");
	else
		printf("(%d)", _get_index_by_node(graph->topo_next_hop[node_index], graph->topo_start_node->topo_ngbr, HELLO_MAX_NEIGHBOR));

	printf("\t%s -----\t|\n", node->topo_idtt);
	for (i = 0; i < HELLO_MAX_NEIGHBOR; i++) {
		if (node->topo_ngbr[i] == NULL) {
			printf("\t\t|(%hhu) --\n", i);
			continue;
		} else {
			outbound_port = _get_index_by_node(node, node->topo_ngbr[i]->topo_ngbr, HELLO_MAX_NEIGHBOR);
			printf("\t\t|(%hhu) --- (%hhu) %s\n", i, outbound_port, node->topo_ngbr[i]->topo_idtt);
		}
	}
}


void nttutil_dump_graph(nettopo_graph_t *graph)
{
	int i;
	printf(" * Nodes Number: %llu\n", (long long unsigned int)(graph->topo_nodes_number));
	printf(" * Start Node: %s\n", graph->topo_start_node->topo_idtt);
	for (i = 0 ; i < graph->topo_nodes_number; i++) {
		_dump_node(graph->topo_nodes[i], graph, i);
		printf("\n");
	}
}

