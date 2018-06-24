#include "memory.h"
#include "Cache.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

struct memory_t {

	Cache L1, L2;
	int L1_access_time;
	int L2_access_time;
	int memory_access_time;
	Write_Policy write_policy;

	/*statistis*/
	int total_time;
	int L1_commands;
	int L2_commands;
	int L1_misses;
	int L2_misses;

};

typedef enum { 
	QUERY_FOUND,
	QUERY_NOT_FOUND
} query_status;

static query_status query_memory(Memory memory,unsigned long int address) {

	Cache L1 = memory->L1;
	Cache L2 = memory->L2;

	// accessing 1st cache
	memory->L1_commands++;
	memory->total_time += memory->L1_access_time;

	if(TryAccess(L1,address) != MISS){
		return QUERY_FOUND;
	}

	memory->L1_misses++;

	// accessing 2nd
	memory->total_time += memory->L2_access_time;
	memory->L2_commands++;

	if(TryAccess(L2,address) == MISS) {
		memory->total_time += memory->memory_access_time;
		memory->L2_misses++;
	}

	return QUERY_NOT_FOUND;
}

static void snoop_cache(Cache cache,unsigned long int address){

	bool isDirty;
	WriteResult res;

	res = removeAddress(cache, address, &isDirty);

	if (res == DIRTY)
		return; // ignore
	// in our implementation we really don't have anything
	// to do with the result of this operation. It is still
	// good to have it though
}

static void insert_address(Memory memory,unsigned long int address,Operation op) {

	Cache L1 = memory->L1;
	Cache L2 = memory->L2;
	unsigned long int lru_address;
	bool isDirty = false;
	WriteResult res;


	if (TryAccess(L2, address) == MISS) {

		if ( op == OP_WRITE && memory->write_policy == NO_WRITE_ALLOC)
			return;

		res = writeAddress(L2 ,address, &lru_address, &isDirty);

		if (res == REPLACED) {
			snoop_cache(L1,lru_address);
		}
	}

	if (op == OP_READ ||
			(op == OP_WRITE && memory->write_policy == WRITE_ALLOC)) {

		res = writeAddress(L1 ,address, &lru_address, &isDirty);
		if (res == REPLACED) { // writeback
			
			if (isDirty) {
				// we update LRU as part of WB
				TryAccess(L2,lru_address);
				res = setDirty(L2, lru_address);
				assert (res == SUCCESS);
			}
		}

		if (op == OP_WRITE)
			setDirty(L1,address);	
	}
}

static void do_write_op(Memory memory,unsigned long int address){

	if(query_memory(memory,address) == QUERY_NOT_FOUND)
		insert_address(memory, address,OP_WRITE);
	else {
		setDirty(memory->L1, address);
	}
}

static void do_read_op(Memory memory,unsigned long int address) {

	if(query_memory(memory,address) == QUERY_NOT_FOUND)
		insert_address(memory, address,OP_READ);
}

Memory CreateMemory(int L1Size, int L2Size, int blockSize, int L1Way, int L2Way,
	int L1Cycles, int L2Cycles, int MemCycles, Write_Policy write) {

	Memory memory;

	memory =(struct memory_t*) calloc(1, sizeof(*memory));

	if (!memory)
		return NULL;

	memory->L1 = CreateCache(L1Size, blockSize, L1Way);
	memory->L2 = CreateCache(L2Size, blockSize, L2Way);

	memory->L1_access_time = L1Cycles;
	memory->L2_access_time = L2Cycles;
	memory->memory_access_time = MemCycles;
	memory->write_policy = write;

	if( !memory->L1 || !memory->L2) {
		DestroyMemory(memory);
		return NULL;
	}

	return memory;
}


void CacheOperation(Memory memory, Operation op, unsigned long int address) {


	switch(op) {

	case OP_WRITE:
		do_write_op(memory,address);
		break;
	case OP_READ:
		do_read_op(memory,address);
		break;
    	}
}


double returnMissRate(Memory memory, int wanted_cache) {
	double L1_misses = (double) memory->L1_misses;
	double L2_misses = (double) memory->L2_misses;
	double L1_ops = (double) memory->L1_commands;
	double L2_ops = (double) memory->L2_commands;

	return (wanted_cache == 1) ? L1_misses/L1_ops : L2_misses/L2_ops;
}

double returnAvgAccTime(Memory memory) {
	double total_time = (double) memory->total_time;
	double total_ops = (double) memory->L1_commands;

	return total_time/total_ops;
}

void DestroyMemory(Memory memory) {

	if (memory->L1)
		ReleaseCache(memory->L1);
	if (memory->L2)
		ReleaseCache(memory->L2);
	free(memory);
}
