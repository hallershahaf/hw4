#pragma once

typedef void *Caches;

typedef enum {
	NO_WRITE = 0,
	WRITE
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
	** A pointer to the Caches struct
*/
Caches CreateCache(int L1Size, int L2Size, int BlockSize, int L1Way, int L2Way,
	int L1Cycles, int L2Cycles, int MemCycles, Write_Policy write);


/* Performs a write/reasd operation on the caches looking for the relevant address
   Input : 
	** caches -> The caches set up we will be using to find the address
	** op -> Whether we are writing of reading
	** address -> the address we want to find
*/
void CacheOperation(Caches caches, Operation op, int address);

/* Returns the miss rate of the relevant cache
   Input :
	** caches -> the Caches struct we want to use
	** index -> whether we want to check L1 or L2
   Output :
	** A number between 0 and 1, the relevant cache's miss-rate
*/
double returnMissRate(Caches caches, int wanted_cache);

/* Returns the Average access time for all read/write commands
   Input : 
	** caches - The caches struct we want to use
   Output : 
	** The average access time to the caches - (total_cycles / op_num)
*/
double returnAvgAccTime(Caches caches);