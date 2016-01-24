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


/*
 * The idea of these hash functions came from paper:
 * 'Skewed-associative caches', Seznec & Bodin
 *
 */ 

uint64_t __shtable_hash(uint64_t idx)
{
	idx = idx & ((1 << SHTABLE_IDX_BITS) - 1);
	idx = (idx | (((idx << SHTABLE_IDX_BITS) ^ (idx << 1)) & (1 << SHTABLE_IDX_BITS))) >> 1;
	return idx;
}

uint64_t __shtable_hash_r(uint64_t idx)
{
	idx = (idx << 1) | (1 & ((idx >> (SHTABLE_IDX_BITS - 1)) ^ (idx >> (SHTABLE_IDX_BITS - 2))));
	idx = idx & ((1 << SHTABLE_IDX_BITS) - 1);
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
		part_a1 =  __shtable_hash(part_a1) ^ __shtable_hash_r(part_a2) ^ part_a1;	}
	part_a2 = (idx >> (SHTABLE_KEY_BITS - SHTABLE_IDX_BITS)) & ((1 << SHTABLE_IDX_BITS) - 1);
	return  __shtable_hash(part_a1) ^ __shtable_hash_r(part_a2) ^ part_a1;
}

void * _shtable_get_helper(shtable_interfaces_t * shti,  uint64_t idxhash_a, uint64_t idxhash_b, uint64_t tag)
{
	uint64_t value;

	if ((value = shti->getentry(shti->table_a, idxhash_a)) != 0) {
		if (tag == ((value >> SHTABLE_IDX_BITS) & ((1 << SHTABLE_TAG_BITS) - 1)))
			return shti->table_a;
	}

	if ((value = shti->getentry(shti->table_b, idxhash_b)) != 0) {
		if (tag == ((value >> SHTABLE_IDX_BITS) & ((1 << SHTABLE_TAG_BITS) - 1)))
			return shti->table_b;
	}

	return NULL;
}

uint64_t shtable_get(shtable_interfaces_t * shti, uint64_t key)
{
	uint64_t idxhash_a;
	uint64_t idxhash_b;
	uint64_t tag;
	void * table;

	tag = key & ((1 << SHTABLE_TAG_BITS) - 1);
	idxhash_a = shti->idxhash_a(key);
	idxhash_b = shti->idxhash_b(key);

	table = _shtable_get_helper(shti, idxhash_a, idxhash_b, tag);

	if (table == shti->table_a) {
		return (shti->getentry(shti->table_a, idxhash_a)) >> (SHTABLE_IDX_BITS + SHTABLE_TAG_BITS);
	} else if (table == shti->table_b) {
		return (shti->getentry(shti->table_b, idxhash_b)) >> (SHTABLE_IDX_BITS + SHTABLE_TAG_BITS);
	} else {
		return -1;
	} 
}

/*
 * 
 * Following figure shows an entry in the hashtable
 * |--------------|------------|------------|
 * |    Value     |     tag    | hashed key |
 * |--------------|------------|------------|
 *                                   /|\
 *                                    |
 *                                  Here stored hash value of another table
 * 
 */
uint64_t _shtable_set_helper(shtable_interfaces_t * shti, uint64_t idxhash_a, uint64_t idxhash_b, uint64_t tagged_value, uint64_t tried)
{
	uint64_t old_entry;
	uint64_t entry;

	if (shti->getentry(shti->table_a, idxhash_a) == 0) {
		entry = (tagged_value << SHTABLE_IDX_BITS) | idxhash_b;
		shti->setentry(shti->table_a, idxhash_a, entry);
	} else if (shti->getentry(shti->table_b, idxhash_b) == 0) {
		entry = (tagged_value << SHTABLE_IDX_BITS) | idxhash_a;
		shti->setentry(shti->table_b, idxhash_b, entry);
	} else {
		if (tried > SHTABLE_MAX_TRY) {
			return -1;
		}
		// If tried odd times, swap out the entry in table a, then put it into table b
		if (tried & 1){
			old_entry = shti->getentry(shti->table_a, idxhash_a);
			entry = (tagged_value << SHTABLE_IDX_BITS) | idxhash_b;
			shti->setentry(shti->table_a, idxhash_a, entry);
			// Get the key hash value of table b
			idxhash_b = old_entry & ((1 << SHTABLE_IDX_BITS) - 1);
		} else {
			old_entry = shti->getentry(shti->table_b, idxhash_b);
			entry = (tagged_value << SHTABLE_IDX_BITS) | idxhash_a;
			shti->setentry(shti->table_b, idxhash_b, entry);
			idxhash_a = old_entry & ((1 << SHTABLE_IDX_BITS) - 1);
		}

		_shtable_set_helper(shti, idxhash_a, idxhash_b, old_entry >> SHTABLE_IDX_BITS, tried + 1);
	}
	return 0;
}

uint64_t shtable_set(shtable_interfaces_t * shti, uint64_t key, uint64_t value)
{
	uint64_t idxhash_a;
	uint64_t idxhash_b;
	uint64_t tag;
	uint64_t tagged_value;
	uint64_t entry;
	void * table;

	tag = key & ((1 << SHTABLE_TAG_BITS) - 1);
	tagged_value = (value << SHTABLE_TAG_BITS) | tag;
	idxhash_a = shti->idxhash_a(key);
	idxhash_b = shti->idxhash_b(key);
	table = _shtable_get_helper(shti, idxhash_a, idxhash_b, tag);

	if (table == shti->table_a) {
		entry = (tagged_value << SHTABLE_IDX_BITS) | idxhash_b;
		shti->setentry(shti->table_a, idxhash_a, entry);
	} else if (table == shti->table_b) {
		entry = (tagged_value << SHTABLE_IDX_BITS) | idxhash_a;
		shti->setentry(shti->table_b, idxhash_b, entry);
	} else {
		return _shtable_set_helper(shti, idxhash_a, idxhash_b, tagged_value, 0);
	}

	return 0;
}

