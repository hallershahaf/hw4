#include "memory.h"
#include "Cache.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

struct memory_t {

	Cache c1, c2;
	int L1_access_time;
	int L2_access_time;
	int memory_access_time;
	Write_Policy write_policy;

	/*statistis*/
	int total_time;
	int overall_commands;
	int L1_misses;
	int L2_misses;

};


static void snoop_cache(Cache cache,unsigned long int address){

	bool isDirty;
	WriteResult res;

	res = removeAddress(cache, address, &isDirty);

	// in our implementation we really don't have anything
	// to do with the result of this operation. It is still
	// good to have it though


}

static void insert_address(Memory memory,unsigned long int address) {

	Cache L1 = memory->c1;
	Cache L2 = memory->c2;
	unsigned long int lru_address;
	bool isDirty = false;
	WriteResult res;


	if (TryAccess(L2, address) == MISS) {

		res = writeAddress(L2 ,address, &lru_address, &isDirty);
		if (res == REPLACED) {
			snoop_cache(L1,lru_address);
		}
	}

	// sanity check
	assert(TryAccess(L1,address) == MISS);


	res = writeAddress(L1 ,address, &lru_address, &isDirty);
	if (res == REPLACED) {
		// inclusion principle must be kept
		assert(TryAccess(L2,lru_address) == HIT);
		writeAddress(L2, lru_address, NULL, NULL);
	}
}

static void do_write_op(Memory memory,unsigned long int address){


}

static void do_read_op(Memory memory,unsigned long int address) {

	Cache L1 = memory->c1;
	Cache L2 = memory->c2;

	// accessing 1st cache 
	memory->total_time += memory->L1_access_time;

	if(TryAccess(L1,address) == HIT)
		return;

	memory->L1_misses++;

	// accessing 2nd
	memory->total_time += memory->L2_access_time;

	if(TryAccess(L1,address) == MISS) {
		memory->total_time += memory->memory_access_time;
		memory->L2_misses++;
	}

	insert_address(memory, address);
}

Memory CreateMemory(int L1Size, int L2Size, int blockSize, int L1Way, int L2Way,
	int L1Cycles, int L2Cycles, int MemCycles, Write_Policy write) {

	Memory memory;

	memory = calloc(1, sizeof(*memory));

	if (!memory)
		return NULL;

	memory->c1 = CreateCache(L1Size, blockSize, L1Way);
	memory->c2 = CreateCache(L2Size, blockSize, L2Way);

	memory->L1_access_time = L1Cycles;
	memory->L2_access_time = L2Cycles;
	memory->memory_access_time = MemCycles;
	memory->write_policy = write;

	if( !memory->c1 || !memory->c2) {
		DestroyMemory(memory);
		return NULL;
	}

	return memory;
}


void CacheOperation(Memory memory, Operation op, unsigned long int address) {

	memory->overall_commands++;

	switch(op) {

	case OP_WRITE:
		do_write_op(memory,address);
		break;
	case OP_READ:
		do_read_op(memory,address);
		break;
    	}

}

void DestroyMemory(Memory memory) {

	if (memory->c1)
		ReleaseCache(memory->c1);
	if (memory->c2)
		ReleaseCache(memory->c2);
	free(memory);
}
