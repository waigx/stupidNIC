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


#include <stdint.h>
#include <stdio.h>

#include <skewedhashtable.h>


#define GET_N_BITS(dat, n) (dat & ((1ULL << n) - 1ULL))


/*
 * The idea of these hash functions came from paper:
 * 'Skewed-associative caches', Seznec & Bodin
 *
 */ 
uint64_t __shtable_hash(uint64_t idx)
{
	idx = GET_N_BITS(idx, SHTABLE_IDX_BITS);
	idx = (idx | (((idx << SHTABLE_IDX_BITS) ^ (idx << 1)) & (1 << SHTABLE_IDX_BITS))) >> 1;
	return idx;
}

uint64_t __shtable_hash_r(uint64_t idx)
{
	idx = (idx << 1) | (1 & ((idx >> (SHTABLE_IDX_BITS - 1)) ^ (idx >> (SHTABLE_IDX_BITS - 2))));
	idx = GET_N_BITS(idx, SHTABLE_IDX_BITS);
	return idx;
}

uint64_t shtable_idxhash_a(uint64_t idx)
{
	uint64_t part_a1;
	uint64_t part_a2;
	int i;
	part_a1 = idx & ((1 << SHTABLE_IDX_BITS) - 1);
	for (i = 1; i < SHTABLE_KEY_BITS / SHTABLE_IDX_BITS; i++) {
		part_a2 = (idx >> (SHTABLE_IDX_BITS * i)) & ((1 << SHTABLE_IDX_BITS) - 1);
		part_a1 =  __shtable_hash(part_a1) ^ __shtable_hash_r(part_a2) ^ part_a2;
	}
	part_a2 = (idx >> (SHTABLE_KEY_BITS - SHTABLE_IDX_BITS)) & ((1 << SHTABLE_IDX_BITS) - 1);
	return  __shtable_hash(part_a1) ^ __shtable_hash_r(part_a2) ^ part_a2;
}

uint64_t shtable_idxhash_b(uint64_t idx)
{
	uint64_t part_a1;
	uint64_t part_a2;
	int i;
	part_a1 = idx & ((1 << SHTABLE_IDX_BITS) - 1);
	for (i = 1; i < SHTABLE_KEY_BITS / SHTABLE_IDX_BITS; i++) {
		part_a2 = (idx >> (SHTABLE_IDX_BITS * i)) & ((1 << SHTABLE_IDX_BITS) - 1);
		part_a1 =  __shtable_hash(part_a1) ^ __shtable_hash_r(part_a2) ^ part_a1;
	}
	part_a2 = (idx >> (SHTABLE_KEY_BITS - SHTABLE_IDX_BITS)) & ((1 << SHTABLE_IDX_BITS) - 1);
	return  __shtable_hash(part_a1) ^ __shtable_hash_r(part_a2) ^ part_a1;
}

uint64_t shtable_idxhash_c(uint64_t idx)
{
	uint64_t part_a1;
	uint64_t part_a2;
	int i;
	part_a1 = idx & ((1 << SHTABLE_IDX_BITS) - 1);
	for (i = 1; i < SHTABLE_KEY_BITS / SHTABLE_IDX_BITS; i++) {
		part_a2 = (idx >> (SHTABLE_IDX_BITS * i)) & ((1 << SHTABLE_IDX_BITS) - 1);
		part_a1 =  __shtable_hash_r(part_a1) ^ __shtable_hash(part_a2) ^ part_a2;
	}
	part_a2 = (idx >> (SHTABLE_KEY_BITS - SHTABLE_IDX_BITS)) & ((1 << SHTABLE_IDX_BITS) - 1);
	return  __shtable_hash_r(part_a1) ^ __shtable_hash(part_a2) ^ part_a2;
}

uint64_t shtable_idxhash_d(uint64_t idx)
{
	uint64_t part_a1;
	uint64_t part_a2;
	int i;
	part_a1 = idx & ((1 << SHTABLE_IDX_BITS) - 1);
	for (i = 1; i < SHTABLE_KEY_BITS / SHTABLE_IDX_BITS; i++) {
		part_a2 = (idx >> (SHTABLE_IDX_BITS * i)) & ((1 << SHTABLE_IDX_BITS) - 1);
		part_a1 =  __shtable_hash_r(part_a1) ^ __shtable_hash(part_a2) ^ part_a1;
	}
	part_a2 = (idx >> (SHTABLE_KEY_BITS - SHTABLE_IDX_BITS)) & ((1 << SHTABLE_IDX_BITS) - 1);
	return  __shtable_hash_r(part_a1) ^ __shtable_hash(part_a2) ^ part_a1;
}

void * _shtable_get_helper(shtable_interfaces_t * shti, uint64_t key)
{
	uint64_t value;

	if ((value = shti->getentry(shti->table_a, shti->idxhash_a(key))) != 0) {
		if (key == GET_N_BITS(value, SHTABLE_KEY_BITS))
			return shti->table_a;
	}

	if ((value = shti->getentry(shti->table_b, shti->idxhash_b(key))) != 0) {
		if (key == GET_N_BITS(value, SHTABLE_KEY_BITS))
			return shti->table_b;
	}

	if ((value = shti->getentry(shti->table_c, shti->idxhash_c(key))) != 0) {
		if (key == GET_N_BITS(value, SHTABLE_KEY_BITS))
			return shti->table_c;
	}

	if ((value = shti->getentry(shti->table_d, shti->idxhash_d(key))) != 0) {
		if (key == GET_N_BITS(value, SHTABLE_KEY_BITS))
			return shti->table_d;
	}

	return NULL;
}

uint64_t shtable_get(shtable_interfaces_t * shti, uint64_t key)
{
	void * table;

	table = _shtable_get_helper(shti, key);

	if (table == shti->table_a) {
		return (shti->getentry(shti->table_a, shti->idxhash_a(key))) >> (SHTABLE_KEY_BITS);
	} else if (table == shti->table_b) {
		return (shti->getentry(shti->table_b, shti->idxhash_b(key))) >> (SHTABLE_KEY_BITS);
	} else if (table == shti->table_c) {
		return (shti->getentry(shti->table_c, shti->idxhash_c(key))) >> (SHTABLE_KEY_BITS);
	} else if (table == shti->table_d) {
		return (shti->getentry(shti->table_d, shti->idxhash_d(key))) >> (SHTABLE_KEY_BITS);
	} else {
		return -1;
	} 
}

/*
 * 
 * Following figure shows an entry in the hashtable
 * |--------------|-------------------------|
 * |    Value     |            key          |
 * |--------------|-------------------------|
 *                                   /|\
 *                                    |
 *                                  Here stored original key
 * 
 */
uint64_t _shtable_set_helper(shtable_interfaces_t * shti, uint64_t entry, uint64_t tried)
{
	uint64_t old_entry;
	uint64_t key;

	key = GET_N_BITS(entry, SHTABLE_KEY_BITS);
	if (shti->getentry(shti->table_a, shti->idxhash_a(key)) == 0) {
		shti->setentry(shti->table_a, shti->idxhash_a(key), entry);
	} else if (shti->getentry(shti->table_b, shti->idxhash_b(key)) == 0) {
		shti->setentry(shti->table_b, shti->idxhash_b(key), entry);
	} else if (shti->getentry(shti->table_c, shti->idxhash_c(key)) == 0) {
		shti->setentry(shti->table_c, shti->idxhash_c(key), entry);
	} else if (shti->getentry(shti->table_d, shti->idxhash_d(key)) == 0) {
		shti->setentry(shti->table_d, shti->idxhash_d(key), entry);
	} else {
		if (tried > SHTABLE_MAX_TRY) {
			return -1;
		}
		// Swap out the entry in one table then put them in next table, in loop
		switch(tried & 0x3) {
		case 0:
			old_entry = shti->getentry(shti->table_d, shti->idxhash_d(key));
			shti->setentry(shti->table_d, shti->idxhash_d(key), entry);
			break;
		case 1:
			old_entry = shti->getentry(shti->table_a, shti->idxhash_a(key));
			shti->setentry(shti->table_a, shti->idxhash_a(key), entry);
			break;
		case 2:
			old_entry = shti->getentry(shti->table_b, shti->idxhash_b(key));
			shti->setentry(shti->table_b, shti->idxhash_b(key), entry);
			break;
		case 3:
			old_entry = shti->getentry(shti->table_c, shti->idxhash_c(key));
			shti->setentry(shti->table_c, shti->idxhash_c(key), entry);
			break;
		default:
			break;
		}
		return _shtable_set_helper(shti, old_entry, tried + 1);
	}
	return 0;
}

uint64_t shtable_set(shtable_interfaces_t * shti, uint64_t key, uint64_t value)
{
	uint64_t entry;
	void * table;

	table = _shtable_get_helper(shti, key);
	entry = (value << SHTABLE_KEY_BITS) | GET_N_BITS(key, SHTABLE_KEY_BITS);

	if (table == shti->table_a) {
		shti->setentry(shti->table_a, shti->idxhash_a(key), entry);
	} else if (table == shti->table_b) {
		shti->setentry(shti->table_b, shti->idxhash_b(key), entry);
	} else if (table == shti->table_c) {
		shti->setentry(shti->table_c, shti->idxhash_c(key), entry);
	} else if (table == shti->table_d) {
		shti->setentry(shti->table_d, shti->idxhash_d(key), entry);
	} else {
		return _shtable_set_helper(shti, entry, 0);
	}
	return 0;
}

