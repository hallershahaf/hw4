#pragma once

typedef void *Cache;

typedef enum {
	MISS = 0,
	HIT
} AccessResult;

typedef enum {
	SUCCESS,
	DIRTY
} AllocResult;


/* Creates a cache.
   Input : 
	** size -> The size of the cache, in bytes (power of two)
	** blockSize -> The size of a memory block, in bytes (power of two)
	** nWay -> The level of association (power of two)
   Output :
	** A pointer to the cache
*/
Cache CreateCache(int size, int blockSize, int nWay);

/* Tries to find a specific address in the cache, whether it's reading or writing
   Input : 
	** cache -> The cache we want to search in
	** address -> The address we want to find
   Output :
	** MISS/HIT - whether the address is in the cache or not
*/
AccessResult TryAccess(Cache cache, int address);

/* Releases all the memory allocated to the cache
   Input :
	** cache -> The cache whose memory we want to release
*/
void ReleaseCache(Cache cache);

