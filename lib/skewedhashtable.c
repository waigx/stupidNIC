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

#include <skewedhashtable.h>


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

uint64_t _shtable_idxhash_a(uint64_t idx)
{
	uint64_t part_a1;
	uint64_t part_a2;
	part_a1 = (idx >> SHTABLE_TAG_BITS) & ((1 << SHTABLE_IDX_BITS) - 1);
	part_a2 = (idx >> SHTABLE_IDX_BITS >> SHTABLE_TAG_BITS) & ((1 << SHTABLE_IDX_BITS) - 1);
	return __shtable_hash(part_a1) ^ __shtable_hash_r(part_a2) ^ part_a2;
}

uint64_t _shtable_idxhash_b(uint64_t idx)
{
	uint64_t part_a1;
	uint64_t part_a2;
	part_a1 = (idx >> SHTABLE_TAG_BITS) & ((1 << SHTABLE_IDX_BITS) - 1);
	part_a2 = (idx >> SHTABLE_IDX_BITS >> SHTABLE_TAG_BITS) & ((1 << SHTABLE_IDX_BITS) - 1);
	return __shtable_hash(part_a1) ^ __shtable_hash_r(part_a2) ^ part_a1;
}

uint64_t shtable_set(shtable_interfaces_t * shti, uint64_t key, uint64_t value)
{
	return 0;
}

uint64_t shtable_get(shtable_interfaces_t * shti, uint64_t key)
{
	return 0;
}


