#include "memory.h"
#include "Cache.h"
#include <stdlib.h>

struct memory_t {

	Cache c1, c2;
	int overall_commands;
	int hit_num;

};

void do_write_op(Memory memory,unsigned long int address){


}

void do_read_op(Memory memory,unsigned long int address){

}

Memory CreateMemory(int L1Size, int L2Size, int blockSize, int L1Way, int L2Way,
	int L1Cycles, int L2Cycles, int MemCycles, Write_Policy write) {

	Memory memory;
	
	memory = calloc(1, sizeof(*memory));
	
	if (!memory)
		return NULL;
	
	memory->c1 = CreateCache(L1Size, blockSize, L1Way);
	memory->c2 = CreateCache(L2Size, blockSize, L2Way);
	
	if( !memory->c1 || !memory->c2) {
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

void DestroyMemory(Memory memory) {

	if (memory->c1)
		ReleaseCache(memory->c1);
	if (memory->c2)
		ReleaseCache(memory->c2);
	free(memory);
}
