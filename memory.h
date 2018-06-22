#ifndef MEMORY_H_
#define MEMORY_H_

typedef struct memory_t *Memory;

typedef enum {
	NO_WRITE_ALLOC = 0,
	WRITE_ALLOC
} Write_Policy;

typedef enum {
	OP_READ = 0,
	OP_WRITE
} Operation;

/* Creates a cache set up, including an L1 cache, and an L2 cache.
   Input :
	** L1Size, L2Size -> The size of the respective caches, in bytes (power of two)
	** BlockSize -> The size of a bloack in the cache (power of two)
	** L1Way, L2Way -> What sort of N-way are the respective caches
	** L1Cycles, L2Cycles, MemCycles -> Num of cycles needed to read from a cache/memory
	** write -> Whether we have write allocation or not
   Output :
	** A pointer to the memory system
*/
Memory CreateMemory(int L1Size, int L2Size, int blockSize, int L1Way, int L2Way,
	int L1Cycles, int L2Cycles, int MemCycles, Write_Policy write);


/* Performs a write/reasd operation on the caches looking for the relevant address
   Input : 
	** mem -> The caches set up we will be using to find the address
	** op -> Whether we are writing of reading
	** address -> the address we want to find
*/
void CacheOperation(Memory mem, Operation op, unsigned long int address);

/* Returns the miss rate of the relevant cache
   Input :
	** mem -> the Caches struct we want to use
	** index -> whether we want to check L1 or L2
   Output :
	** A number between 0 and 1, the relevant cache's miss-rate
*/
double returnMissRate(Memory mem, int wanted_cache);

/* Returns the Average access time for all read/write commands
   Input : 
	** mem - The caches struct we want to use
   Output : 
	** The average access time to the caches - (total_cycles / op_num)
*/
double returnAvgAccTime(Memory mem);


/* Releases all the memory of the relevant memory system
   Input :
	** mem - The memory system to release
*/
void DestroyMemory(Memory mem);

#endif /* MEMORY_H_ */
