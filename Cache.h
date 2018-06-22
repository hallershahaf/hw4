#ifndef CACHE_H_
#define CACHE_H_

#include <stdbool.h>

typedef void *Cache;

typedef enum {
	MISS = 0,
	HIT,
	HIT_DIRTY
} AccessResult;

typedef enum {
	SUCESS = 0,
	REPLACED,
	DIRTY,
	NOT_FOUND
} WriteResult;


/* Creates a cache.
   Input :
	** size -> The size of the cache, in bytes (power of two)
	** blockSize -> The size of a memory block, in bytes (power of two)
	** nWay -> The level of association (power of two)
   Output :
	** A pointer to the cache
*/
Cache CreateCache(int size, int blockSize, int nWay);

/* Tries to find a specific address in the cache, whether it's for reading or writing
   Input :
	** cache -> The cache we want to search in
	** address -> The address we want to find
   Output :
	** MISS/HIT - whether the address is in the cache or not
*/
AccessResult TryAccess(Cache cache, unsigned long int address);

/* Writes a an address to cache. If the cache is full, the function
 * 	deletes the least recently used address and returns its
 * 	address.
 * Input:
 * 	** cache -> The cache to write to
 * 	** address -> the address we want to write
 * 	** lru_address -> the least recently used address
 * 	** isDirty -> 'true' if the lru was dirty
 *
 * Output:
 * 	SUCCESS - writing was successful
 * 	REPLACED - the address replaced another address
 *
 */
WriteResult writeAddress(Cache cache, unsigned long int address,
		unsigned long int *lru_address, bool *isDirty);

/* Removes an address from cache.
 * returns isDirty = 'true' if the address was dirty
 * Input:
 * 	** cache -> The cache to delete from
 * 	** address -> the address we want to delete 
 * 	** isDirty -> 'true' if the removed address was dirty
 *
 * Output:
 * 	SUCCESS - deletion was successful
 * 	DIRTY - deletion was successful but the address
 * 		was dirty
 *	NOT_FOUND - this address isn't in cache
 *
 */
WriteResult removeAddress(Cache cache,unsigned long int address,
		bool *isDirty);


/* Releases all the memory allocated to the cache
   Input :
	** cache -> The cache whose memory we want to release
*/
void ReleaseCache(Cache cache);

#endif /* CACHE_H_ */
