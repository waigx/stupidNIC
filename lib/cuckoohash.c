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

#include <cuckoohash.h>


#define GET_N_BITS(dat, n) (dat & ((1ULL << n) - 1ULL))


/*
 * The idea of these hash functions came from paper:
 * 'Skewed-associative caches', Seznec & Bodin
 *
 */ 
uint64_t __ckhtable_hash(uint64_t idx)
{
	idx = GET_N_BITS(idx, CKHTABLE_IDX_BITS);
	idx = (idx | (((idx << CKHTABLE_IDX_BITS) ^ (idx << 1)) & (1 << CKHTABLE_IDX_BITS))) >> 1;
	return idx;
}

uint64_t __ckhtable_hash_r(uint64_t idx)
{
	idx = (idx << 1) | (1 & ((idx >> (CKHTABLE_IDX_BITS - 1)) ^ (idx >> (CKHTABLE_IDX_BITS - 2))));
	idx = GET_N_BITS(idx, CKHTABLE_IDX_BITS);
	return idx;
}

uint64_t ckhtable_idxhash_a(uint64_t idx)
{
	uint64_t part_a1;
	uint64_t part_a2;
	int i;
	part_a1 = idx & ((1 << CKHTABLE_IDX_BITS) - 1);
	for (i = 1; i < CKHTABLE_KEY_BITS / CKHTABLE_IDX_BITS; i++) {
		part_a2 = GET_N_BITS((idx >> (CKHTABLE_IDX_BITS * i)), CKHTABLE_IDX_BITS);
		part_a1 =  __ckhtable_hash(part_a1) ^ __ckhtable_hash_r(part_a2) ^ part_a2;
	}
	part_a2 = GET_N_BITS((idx >> (CKHTABLE_KEY_BITS - CKHTABLE_IDX_BITS)), CKHTABLE_IDX_BITS);
	return  __ckhtable_hash(part_a1) ^ __ckhtable_hash_r(part_a2) ^ part_a2;
}

uint64_t ckhtable_idxhash_b(uint64_t idx)
{
	uint64_t part_a1;
	uint64_t part_a2;
	int i;
	part_a1 = idx & ((1 << CKHTABLE_IDX_BITS) - 1);
	for (i = 1; i < CKHTABLE_KEY_BITS / CKHTABLE_IDX_BITS; i++) {
		part_a2 = GET_N_BITS((idx >> (CKHTABLE_IDX_BITS * i)), CKHTABLE_IDX_BITS);
		part_a1 =  __ckhtable_hash(part_a1) ^ __ckhtable_hash_r(part_a2) ^ part_a1;
	}
	part_a2 = GET_N_BITS((idx >> (CKHTABLE_KEY_BITS - CKHTABLE_IDX_BITS)), CKHTABLE_IDX_BITS);
	return  __ckhtable_hash(part_a1) ^ __ckhtable_hash_r(part_a2) ^ part_a1;
}

uint64_t ckhtable_idxhash_c(uint64_t idx)
{
	uint64_t part_a1;
	uint64_t part_a2;
	int i;
	part_a1 = idx & ((1 << CKHTABLE_IDX_BITS) - 1);
	for (i = 1; i < CKHTABLE_KEY_BITS / CKHTABLE_IDX_BITS; i++) {
		part_a2 = GET_N_BITS((idx >> (CKHTABLE_IDX_BITS * i)), CKHTABLE_IDX_BITS);
		part_a1 =  __ckhtable_hash_r(part_a1) ^ __ckhtable_hash(part_a2) ^ part_a2;
	}
	part_a2 = GET_N_BITS((idx >> (CKHTABLE_KEY_BITS - CKHTABLE_IDX_BITS)), CKHTABLE_IDX_BITS);
	return  __ckhtable_hash_r(part_a1) ^ __ckhtable_hash(part_a2) ^ part_a2;
}

uint64_t ckhtable_idxhash_d(uint64_t idx)
{
	uint64_t part_a1;
	uint64_t part_a2;
	int i;
	part_a1 = idx & ((1 << CKHTABLE_IDX_BITS) - 1);
	for (i = 1; i < CKHTABLE_KEY_BITS / CKHTABLE_IDX_BITS; i++) {
		part_a2 = GET_N_BITS((idx >> (CKHTABLE_IDX_BITS * i)), CKHTABLE_IDX_BITS);
		part_a1 =  __ckhtable_hash_r(part_a1) ^ __ckhtable_hash(part_a2) ^ part_a1;
	}
	part_a2 = GET_N_BITS((idx >> (CKHTABLE_KEY_BITS - CKHTABLE_IDX_BITS)), CKHTABLE_IDX_BITS);
	return  __ckhtable_hash_r(part_a1) ^ __ckhtable_hash(part_a2) ^ part_a1;
}

void * _ckhtable_get_helper(ckhtable_interfaces_t * ckhti, uint64_t key)
{
	uint64_t value;

	if ((value = ckhti->getentry(ckhti->table_a, ckhti->idxhash_a(key))) != 0) {
		if (key == GET_N_BITS(value, CKHTABLE_KEY_BITS))
			return ckhti->table_a;
	}

	if ((value = ckhti->getentry(ckhti->table_b, ckhti->idxhash_b(key))) != 0) {
		if (key == GET_N_BITS(value, CKHTABLE_KEY_BITS))
			return ckhti->table_b;
	}

	if ((value = ckhti->getentry(ckhti->table_c, ckhti->idxhash_c(key))) != 0) {
		if (key == GET_N_BITS(value, CKHTABLE_KEY_BITS))
			return ckhti->table_c;
	}

	if ((value = ckhti->getentry(ckhti->table_d, ckhti->idxhash_d(key))) != 0) {
		if (key == GET_N_BITS(value, CKHTABLE_KEY_BITS))
			return ckhti->table_d;
	}

	return NULL;
}

uint64_t ckhtable_get(ckhtable_interfaces_t * ckhti, uint64_t key)
{
	void * table;

	table = _ckhtable_get_helper(ckhti, key);

	if (table == ckhti->table_a) {
		return (ckhti->getentry(ckhti->table_a, ckhti->idxhash_a(key))) >> (CKHTABLE_KEY_BITS);
	} else if (table == ckhti->table_b) {
		return (ckhti->getentry(ckhti->table_b, ckhti->idxhash_b(key))) >> (CKHTABLE_KEY_BITS);
	} else if (table == ckhti->table_c) {
		return (ckhti->getentry(ckhti->table_c, ckhti->idxhash_c(key))) >> (CKHTABLE_KEY_BITS);
	} else if (table == ckhti->table_d) {
		return (ckhti->getentry(ckhti->table_d, ckhti->idxhash_d(key))) >> (CKHTABLE_KEY_BITS);
	} else {
		return -1;
	} 
}

/*
 * 
 * Following figure shows an entry in the hackhtable
 * |--------------|-------------------------|
 * |    Value     |            key          |
 * |--------------|-------------------------|
 *                                   /|\
 *                                    |
 *                                  Here stored original key
 * 
 */
uint64_t _ckhtable_set_helper(ckhtable_interfaces_t * ckhti, uint64_t entry, uint64_t tried)
{
	uint64_t old_entry;
	uint64_t key;

	key = GET_N_BITS(entry, CKHTABLE_KEY_BITS);
	if (ckhti->getentry(ckhti->table_a, ckhti->idxhash_a(key)) == 0) {
		ckhti->setentry(ckhti->table_a, ckhti->idxhash_a(key), entry);
	} else if (ckhti->getentry(ckhti->table_b, ckhti->idxhash_b(key)) == 0) {
		ckhti->setentry(ckhti->table_b, ckhti->idxhash_b(key), entry);
	} else if (ckhti->getentry(ckhti->table_c, ckhti->idxhash_c(key)) == 0) {
		ckhti->setentry(ckhti->table_c, ckhti->idxhash_c(key), entry);
	} else if (ckhti->getentry(ckhti->table_d, ckhti->idxhash_d(key)) == 0) {
		ckhti->setentry(ckhti->table_d, ckhti->idxhash_d(key), entry);
	} else {
		if (tried > CKHTABLE_MAX_TRY) {
			return -1;
		}
		// Swap out the entry in one table then put them in next table, in loop
		switch(tried & 0x3) {
		case 0:
			old_entry = ckhti->getentry(ckhti->table_d, ckhti->idxhash_d(key));
			ckhti->setentry(ckhti->table_d, ckhti->idxhash_d(key), entry);
			break;
		case 1:
			old_entry = ckhti->getentry(ckhti->table_a, ckhti->idxhash_a(key));
			ckhti->setentry(ckhti->table_a, ckhti->idxhash_a(key), entry);
			break;
		case 2:
			old_entry = ckhti->getentry(ckhti->table_b, ckhti->idxhash_b(key));
			ckhti->setentry(ckhti->table_b, ckhti->idxhash_b(key), entry);
			break;
		case 3:
			old_entry = ckhti->getentry(ckhti->table_c, ckhti->idxhash_c(key));
			ckhti->setentry(ckhti->table_c, ckhti->idxhash_c(key), entry);
			break;
		default:
			break;
		}
		return _ckhtable_set_helper(ckhti, old_entry, tried + 1);
	}
	return 0;
}

uint64_t ckhtable_set(ckhtable_interfaces_t * ckhti, uint64_t key, uint64_t value)
{
	uint64_t entry;
	void * table;

	table = _ckhtable_get_helper(ckhti, key);
	entry = (value << CKHTABLE_KEY_BITS) | GET_N_BITS(key, CKHTABLE_KEY_BITS);

	if (table == ckhti->table_a) {
		ckhti->setentry(ckhti->table_a, ckhti->idxhash_a(key), entry);
	} else if (table == ckhti->table_b) {
		ckhti->setentry(ckhti->table_b, ckhti->idxhash_b(key), entry);
	} else if (table == ckhti->table_c) {
		ckhti->setentry(ckhti->table_c, ckhti->idxhash_c(key), entry);
	} else if (table == ckhti->table_d) {
		ckhti->setentry(ckhti->table_d, ckhti->idxhash_d(key), entry);
	} else {
		return _ckhtable_set_helper(ckhti, entry, 0);
	}
	return 0;
}

