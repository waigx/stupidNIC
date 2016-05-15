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
extern nettopo_node_t node_table[NETTOPO_MAX_NODE];


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


nettopo_node_t * _get_node_by_idtt(unsigned char * node_idtt, nettopo_node_t ** node_array, uint64_t node_array_len)
{
	int i;
	for (i = 0; i < node_array_len; i++)
		if (_is_same_node_idtt(node_array[i]->topo_idtt, node_idtt) == true)
			return node_array[i];
	return NULL;
}


int _get_index_by_node(nettopo_node_t * node, nettopo_node_t ** node_array, uint64_t node_array_len)
{
	int i;
	for (i = 0; i < node_array_len; i++)
		if (node_array[i] == node)
			return i;
	return -1;
}


void * _malloc_node()
{
	int i;
	for (i = 0; i < NETTOPO_MAX_NODE; i++) {
		if (_is_blank_idtt(node_table[i].topo_idtt)){
			return node_table + i;
		}
	}
	return NULL;
}


void _free_node(nettopo_node_t *node){
	memset(node->topo_idtt, 0, HELLO_IDENTITY_LEN);
}


nettopo_node_t * _create_new_node(unsigned char * node_idtt)
{
	int i;
	nettopo_node_t * node;

	node = _malloc_node();	
	memcpy(node->topo_idtt, node_idtt, HELLO_IDENTITY_LEN);
	for (i = 0; i < HELLO_MAX_NEIGHBOR; i++)
		node->topo_ngbr[i] = NULL;

	return node;
}


int _insert_node_to_graph(nettopo_node_t * node)
{
	nettopo_node_t * stock_node;

	if (nettopo_graph.topo_nodes_number == NETTOPO_MAX_NODE)
		return -1;

	stock_node = _get_node_by_idtt(node->topo_idtt, nettopo_graph.topo_nodes, nettopo_graph.topo_nodes_number);
	if (stock_node != NULL)
		return -1;

	nettopo_graph.topo_nodes[nettopo_graph.topo_nodes_number] = node;
	nettopo_graph.topo_next_hop[nettopo_graph.topo_nodes_number] = NULL;
	nettopo_graph.topo_nodes_number += 1;

	return 0;
}


int _remove_node_from_graph(nettopo_node_t * node)
{
	int i;

	if (nettopo_graph.topo_nodes_number == 0)
		return -1;

	for (i = 0; i < nettopo_graph.topo_nodes_number; i++)
		if (node == nettopo_graph.topo_nodes[i])
			break;

	if (i == nettopo_graph.topo_nodes_number)
		return -1;

	for (; i < nettopo_graph.topo_nodes_number - 1; i++) {
		nettopo_graph.topo_nodes[i] = nettopo_graph.topo_nodes[i + 1];
		nettopo_graph.topo_next_hop[i] = nettopo_graph.topo_next_hop[i + 1];
	}

	nettopo_graph.topo_nodes[i] = NULL;
	nettopo_graph.topo_nodes_number -= 1;

	_free_node(node);

	return 0;
}


int _nodecmp(const void * a, const void * b)
{
	int i;
	nettopo_node_t ** node_a = (nettopo_node_t **)a;
	nettopo_node_t ** node_b = (nettopo_node_t **)b;

	for (i = 0; i < HELLO_IDENTITY_LEN; i++) {
		if ((*node_a)->topo_idtt[i] > (*node_b)->topo_idtt[i])
			return 1;
		if ((*node_a)->topo_idtt[i] < (*node_b)->topo_idtt[i])
			return -1;
	}	
	return 0;
}


void nettopo_sort(nettopo_graph_t * graph)
{
	qsort(graph->topo_nodes, graph->topo_nodes_number, sizeof(nettopo_node_t *), _nodecmp);
	return;
}


uint64_t nettopo_run_bfs(nettopo_graph_t * graph, nettopo_node_t * start_node, nettopo_node_t * relay_node)
{
	int i, j;
	bool visited_nodes[NETTOPO_MAX_NODE];
	bool next_nodes[NETTOPO_MAX_NODE];
	bool next_nodes_temp[NETTOPO_MAX_NODE];
	int current_node_idx;
	int next_nodes_num;
	uint64_t ports;
	nettopo_node_t * current_node;

	for (i = 0; i < graph->topo_nodes_number; i++) {
		visited_nodes[i] = false;
		next_nodes[i] = false;
		next_nodes_temp[i] = false;
	}

	current_node = start_node;
	current_node_idx = _get_index_by_node(current_node, graph->topo_nodes, graph->topo_nodes_number);
	next_nodes[current_node_idx] = true;
	next_nodes_num = 1;
	
	while (next_nodes_num != 0) {
		next_nodes_num = 0;
		for (i = 0; i < graph->topo_nodes_number; i++) {
			if (next_nodes[i]) {
				ports = 0;
				for (j = 0; j < HELLO_MAX_NEIGHBOR; j ++) {
					current_node = graph->topo_nodes[i]->topo_ngbr[j];
					if (current_node != NULL) {
						current_node_idx = _get_index_by_node(current_node, graph->topo_nodes, graph->topo_nodes_number);
					} else {
						continue;
					}
					if (visited_nodes[current_node_idx] || next_nodes[current_node_idx]) {
						continue;
					} else {
						next_nodes_num += 1;
						next_nodes_temp[current_node_idx] = true;
						ports <<= 4;
						ports |= j;
					}
				}
				if (graph->topo_nodes[i] == relay_node)
					return ports;
			}
		}

		for (i = 0; i < graph->topo_nodes_number; i++) {
			next_nodes[i] = next_nodes_temp[i];
		}
	}

	return -1;	
}


void nettopo_run_dijkstra(nettopo_graph_t * graph, nettopo_node_t * start_node)
{
	int i;
	int current_node_idx;
	int temp_node_idx;
	uint64_t dist;
	nettopo_node_t * current_node;
	nettopo_node_t * temp_node;
	bool visited_nodes[NETTOPO_MAX_NODE];
	uint64_t current_dists[NETTOPO_MAX_NODE];
	
	for (i = 0; i < graph->topo_nodes_number; i++) {
		visited_nodes[i] = false;
		current_dists[i] = UINT64_MAX;
	}

	current_node = start_node;
	current_node_idx = _get_index_by_node(current_node, graph->topo_nodes, graph->topo_nodes_number);
	current_dists[current_node_idx] = 0;
	graph->topo_next_hop[current_node_idx] = start_node;
	graph->topo_start_node = start_node;


	while (visited_nodes[current_node_idx] == false) {
		visited_nodes[current_node_idx] = true;
		for (i = 0; i < HELLO_MAX_NEIGHBOR; i++) {
			temp_node = current_node->topo_ngbr[i];
			if (temp_node == NULL)
				continue;
			temp_node_idx = _get_index_by_node(temp_node, graph->topo_nodes, graph->topo_nodes_number);
			if (current_dists[current_node_idx] + 1 < current_dists[temp_node_idx]) {
				current_dists[temp_node_idx] = current_dists[current_node_idx] + 1;
				if (graph->topo_next_hop[current_node_idx] == start_node)
					graph->topo_next_hop[temp_node_idx] = temp_node;
				else
					graph->topo_next_hop[temp_node_idx] = graph->topo_next_hop[current_node_idx];

			}
		}
		dist = UINT64_MAX;
		for (i = 0; i < graph->topo_nodes_number; i++) {
			if ((visited_nodes[i] == false) && (current_dists[i] < dist)) {
				current_node = graph->topo_nodes[i];
				dist = current_dists[i];
			}
		}
		current_node_idx = _get_index_by_node(current_node, graph->topo_nodes, graph->topo_nodes_number);
	}
}


int nettopo_update_graph(unsigned char * node_idtt, unsigned char * raw_ngbr_idtts, unsigned char * raw_ngbr_outbound_ports)
{
	int i, j;
	nettopo_node_t * stock_node;
	nettopo_node_t * temp_node;

	stock_node = _get_node_by_idtt(node_idtt, nettopo_graph.topo_nodes, nettopo_graph.topo_nodes_number);

	if (stock_node == NULL) {
		// If the node does not exist in the graph, create a new one
		stock_node = _create_new_node(node_idtt);
		// Insert new node into current graph
		_insert_node_to_graph(stock_node);
	} else {
		// Clean orginal node pairs information
		for (i = 0; i < HELLO_MAX_NEIGHBOR; i++) {
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
		if (_is_blank_idtt(raw_ngbr_idtts + i * HELLO_IDENTITY_LEN) == true) {
			stock_node->topo_ngbr[i] = NULL;
			continue;
		}
		temp_node = _get_node_by_idtt(raw_ngbr_idtts + i * HELLO_IDENTITY_LEN, nettopo_graph.topo_nodes, nettopo_graph.topo_nodes_number);
		if (temp_node == NULL) {
			temp_node = _create_new_node(raw_ngbr_idtts + i * HELLO_IDENTITY_LEN);
			_insert_node_to_graph(temp_node);
		}
		// Update node pairing information
		temp_node->topo_ngbr[(uint8_t)(raw_ngbr_outbound_ports[i])] = stock_node;
		stock_node->topo_ngbr[i] = temp_node;
	}

	return 0;
}




