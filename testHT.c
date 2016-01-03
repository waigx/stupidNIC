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


#include <unistd.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <skewedhashtable.h>

#define TABLE_SIZE (1 << SHTABLE_IDX_BITS)


uint64_t get_entry(void *table, uint64_t idx)
{
	uint64_t *table_ptr = (uint64_t *)table;
	return table_ptr[idx];
}


uint64_t set_entry(void *table, uint64_t idx, uint64_t entry)
{
	uint64_t *table_ptr = (uint64_t *)table;
	table_ptr[idx] = entry;

	return table_ptr[idx];
}


void dump_bin(uint64_t idx)
{
	int i;
	for (i = 63; i >=0; i--) printf("%d ", (int)((idx >> i) & 1));
	printf("\n");
	return;
}

void dump_ety(uint64_t entry)
{
	printf("%llu ", (long long unsigned int)(entry >> (SHTABLE_IDX_BITS + SHTABLE_TAG_BITS)));
	printf("%llu ", (long long unsigned int)((entry >> (SHTABLE_IDX_BITS )) & ((1 << SHTABLE_TAG_BITS) - 1)));
	printf("%llu", (long long unsigned int)(entry  & ((1 << SHTABLE_IDX_BITS) - 1)));
}

int main(int argc, char *argv[], char *envp[])
{
	char op;
	uint64_t key;
	uint64_t val;
	int i;
	uint64_t table_a[TABLE_SIZE] = {0};
	uint64_t table_b[TABLE_SIZE] = {0};
	shtable_interfaces_t shti;

	shti.table_a = table_a;
	shti.table_b = table_b;
	shti.idxhash_a = shtable_idxhash_a;
	shti.idxhash_b = shtable_idxhash_b;
	shti.getentry = get_entry;
	shti.setentry = set_entry;
	
	printf("Input mode (w, r, d, q): ");
	while (1) {
		switch ((op = getchar())) {
		case 'w':
			printf("Input [key, value]: ");
			scanf("%p, %d", (void **)&key, (int *)&val);
			shtable_set(&shti, key, val);
			break;
		case 'r':
			printf("Input [key]: ");
			scanf("%p", (void **)&key);
			printf("%d\n", (int)shtable_get(&shti, key));
			break;
		case 'd':
			printf("--Table A--\t\t--Table B--\n");
			for (i = 0; i < TABLE_SIZE; i ++) {
				dump_ety(table_a[i]);
				printf("\t\t\t");
				dump_ety(table_b[i]);
				printf("\n");
			}
			break;
		case 'q':
			return 0;
			break;
		default:
			printf("Input mode (w, r, d): ");
			break;
		}
	}
	printf("%d\n", (int)shtable_get(&shti, 0xfbbff));

	return 0;
}
