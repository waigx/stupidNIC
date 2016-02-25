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


#ifndef __NETTOPO_H
#define __NETTOPO_H

#include <hello.h>
#include <stdint.h>


#define NETTOPO_MAX_NODE				    32
#define NETTOPO_DEFAULT_WEIGTH				     1


typedef struct nettopo_node {
	unsigned char			topo_idtt[HELLO_IDENTITY_LEN];
	struct nettopo_node *		topo_ngbr[HELLO_MAX_NEIGHBOR];
} nettopo_node_t;


typedef struct nettopo_graph {
	uint64_t			topo_nodes_number;
	nettopo_node_t *		topo_nodes[NETTOPO_MAX_NODE];
	nettopo_node_t *		topo_next_hop[NETTOPO_MAX_NODE];
} nettopo_graph_t;

nettopo_graph_t nettopo_graph;
nettopo_node_t node_table[NETTOPO_MAX_NODE];

int nettopo_update_graph(unsigned char *, unsigned char *, unsigned char *);
void nettopo_run_dijkstra(nettopo_graph_t *, nettopo_node_t *);

#endif
