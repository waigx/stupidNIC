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


#ifndef __SKEWEDHASHTABLE_H
#define __SKEWEDHASHTABLE_H

#include <stdint.h>


#define SHTABLE_IDX_BITS			8
#define SHTABLE_TAG_BITS			8
#define SHTABLE_KEY_BITS		       48
#define SHTABLE_MAX_TRY			       99


typedef struct shtable_interfaces {
	// define two tables for storage
	void * table_a;
	void * table_b;
	// define two hash functions for index hash
	uint64_t (*idxhash_a)(uint64_t idx);
	uint64_t (*idxhash_b)(uint64_t idx);
	// define how to set/get entry from table
	uint64_t (*getentry)(void *table, uint64_t idx);
	uint64_t (*setentry)(void *table, uint64_t idx, uint64_t entry);
} shtable_interfaces_t;

// Provided two hash functions
uint64_t shtable_idxhash_a(uint64_t);
uint64_t shtable_idxhash_b(uint64_t);

uint64_t shtable_set(shtable_interfaces_t *, uint64_t, uint64_t);
uint64_t shtable_get(shtable_interfaces_t *, uint64_t);


#endif

