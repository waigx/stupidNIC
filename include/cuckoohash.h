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


#ifndef __CUCKOOHASH_H
#define __CUCKOOHASH_H

#include <stdint.h>


#define CKHTABLE_IDX_BITS			6
#define CKHTABLE_KEY_BITS		       48
#define CKHTABLE_MAX_TRY		      256


typedef struct ckhtable_interfaces {
	// define four tables for storage
	void * table_a;
	void * table_b;
	void * table_c;
	void * table_d;
	// define four hash functions for index hash
	uint64_t (*idxhash_a)(uint64_t idx);
	uint64_t (*idxhash_b)(uint64_t idx);
	uint64_t (*idxhash_c)(uint64_t idx);
	uint64_t (*idxhash_d)(uint64_t idx);
	// define how to set/get entry from table
	uint64_t (*getentry)(void *table, uint64_t idx);
	uint64_t (*setentry)(void *table, uint64_t idx, uint64_t entry);
} ckhtable_interfaces_t;

// Provided four hash functions
uint64_t ckhtable_idxhash_a(uint64_t);
uint64_t ckhtable_idxhash_b(uint64_t);
uint64_t ckhtable_idxhash_c(uint64_t);
uint64_t ckhtable_idxhash_d(uint64_t);

uint64_t ckhtable_set(ckhtable_interfaces_t *, uint64_t, uint64_t);
uint64_t ckhtable_get(ckhtable_interfaces_t *, uint64_t);


#endif

