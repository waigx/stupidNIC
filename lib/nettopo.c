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


#include <hello.h>
#include <nettopo.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

extern nettopo_graph_t nettopo_graph;


bool _is_same_node_idtt(unsigned char * node_1, unsigned char * node_2)
{
	int i;
	for (i = 0; i < HELLO_IDENTITY_LEN; i++)
		if (*(node_1 + i) != *(node_2 + i))
			return false;
	return true;
}


bool _is_blank_idtt(unsigned char * node_idtt)
{
	unsigned char blank_node_idtt[HELLO_IDENTITY_LEN] = {0};
	return _is_same_node_idtt(blank_node_idtt, node_idtt);
}


bool _is_unliked_node(nettopo_node_t * node)
{
	int i;
	for (i = 0; i < HELLO_MAX_NEIGHBOR; i++)
		if (node->topo_ngbr[i] != NULL)
			return false;
	return true;
}


void _insert_node_to_graph(nettopo_node_t * node)
{
	// TODO
}


void _remove_node_from_graph(nettopo_node_t * node)
{
	// TODO
}


nettopo_node_t * _get_node_by_idtt(unsigned char * node_idtt, nettopo_node_t ** node_array, uint64_t node_array_len)
{
	int i;
	for (i = 0; i < node_array_len; i++)
		if (_is_same_node_idtt(node_array[i]->topo_idtt, node_idtt) == true)
			return node_array[i];
	return NULL;
}


void _run_dijkstra(nettopo_node_t * start_node, nettopo_graph_t * graph)
{
	// TODO
}


int nettopo_update_graph(unsigned char * node_idtt, unsigned char * raw_ngbr_idtts, unsigned char * raw_ngbr_outbound_ports)
{
	int i, j;
	nettopo_node_t * stock_node;
	nettopo_node_t * temp_node;

	stock_node = _get_node_by_idtt(node_idtt, nettopo_graph.topo_nodes, nettopo_graph.topo_nodes_number);

	if (stock_node == NULL) {
		// If the node does not exist in the graph, create a new one
		stock_node = malloc(sizeof(nettopo_node_t));	
		memcpy(stock_node->topo_idtt, node_idtt, HELLO_IDENTITY_LEN);
		for (i = 0; i < HELLO_MAX_NEIGHBOR; i++)
			stock_node->topo_ngbr[i] = NULL;
		// Insert new node into current graph
		_insert_node_to_graph(stock_node);
	} else {
		for (i = 0; i < HELLO_MAX_NEIGHBOR; i++) {
			// Clean orginal node pairs information
			temp_node = stock_node->topo_ngbr[i];
			if (temp_node != NULL) {
				for (j = 0; j < HELLO_MAX_NEIGHBOR; j++) {
					if (temp_node->topo_ngbr[j] == stock_node) {
						temp_node->topo_ngbr[j] = NULL;
					}
				}
				// Remove the paired node if it is single
				if (_is_unliked_node(temp_node))
					_remove_node_from_graph(temp_node);
			}
		}
	}
	// Updates all pairs informations
	for (i = 0; i < HELLO_MAX_NEIGHBOR; i++) {
		temp_node = _get_node_by_idtt(raw_ngbr_idtts + i * HELLO_IDENTITY_LEN, nettopo_graph.topo_nodes, nettopo_graph.topo_nodes_number);
		if (temp_node == NULL) {
			temp_node = malloc(sizeof(nettopo_node_t));	
			memcpy(temp_node->topo_idtt, raw_ngbr_idtts + i * HELLO_IDENTITY_LEN, HELLO_IDENTITY_LEN);
			_insert_node_to_graph(temp_node);
		}
		// Update node pairing information
		temp_node->topo_ngbr[(uint8_t)(raw_ngbr_outbound_ports[i])] = stock_node;
		stock_node->topo_ngbr[i] = temp_node;
	}

	return 0;
}

