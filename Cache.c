
#include "Cache.h"
#include "list.h"
#include <math.h>

// Defines and stuff

#define ADDRESS_BITS 32
#define FULL_MASK 0xffffffff

typedef struct list_head list_head;

typedef enum {
	INVALID,
	VALID,
	DIRTY
} State;

typedef struct {
	unsigned int tag;
	State state;
	list_head LRU;
} _way;

typedef struct {
	_way *Ways;
	list_head ghost;
} _set;

typedef struct {
	unsigned int numOfWays;
	unsigned int setBits;
	unsigned int blockBits;
	unsigned int tagBits;
	_set *Sets;
} _cache;


// Actual Code

int Pow2(unsigned int exp) {
	int result = 1;
	while (exp != 0) {
		result *= 2;
		exp--;
	}
	return result;
}

// Powers of two
Cache CreateCache(unsigned int size, unsigned int blockSize, unsigned int nWay) {

	int NumOfBlocks = size - blockSize; // Power of two
	int PowOfSets = NumOfBlocks - nWay; // Power of two
	int NumOfSets = Pow2(PowOfSets); // Power of two

	// Cache Init
	_cache *cache = malloc(sizeof(cache));
	if (!cache)
		return NULL;
	cache->setBits = PowOfSets;
	cache->blockBits = blockSize;
	cache->tagBits = ADDRESS_BITS - cache->blockBits - cache->setBits;
	cache->numOfWays = Pow2(nWay);
	cache->Sets = malloc(sizeof(_set) * NumOfSets);
	if (!cache->Sets) {
		free(cache);
		return NULL;
	}

	// Set + Way Init
	_set *Sets = cache->Sets;
	for (int i = 0; i < NumOfSets; i++) {
		Sets[i].Ways = malloc(sizeof(_way) * NumOfWays);
		if (!Sets[i].Ways) {
			for (int j = 0; j < i; j++) {
				free(Sets[j].Ways);
			}
			free(Sets);
			free(cache);
			return NULL;
		}
		INIT_LIST_HEAD(&(Sets[i].ghost))
		for (int j = 0; j < NumOfWays; j++) {
			Sets[i].Ways[j].state = INVALID;
			Sets[i].Ways[j].tag = 0;
			list_add(&(Sets[i].Ways[j].LRU), &(Sets[i].ghost));
		}
	}

	return cache;
}

AccessResult TryAccess(Cache cache, unsigned long int address) {
	_cache *cur_cache = (_cache*)cache;

	unsigned int tag = address >> (cur_cache->blockBits + cur_cache->setBits);
	unsigned int set_index = (address & (FULL_MASK >> cur_cache->tagBits)) >> cur_cache->blockBits;
	_set *cur_set = &(cur_cache->Sets[set_index]);
	for (int i = 0; i < cur_cache->numOfWays; j++) {
		_way *cur_way = &(cur_set.Ways[j]);
		if (cur_way->tag == tag) {
			if (cur_way->state != INVALID) {
				list_del(&(cur_way->LRU));
				list_add_tail(&(cur_way->LRU), &(cur_set->ghost));
			}
			if (cur_way->state == VALID)
				return HIT;
			if (cur_way->state == DIRTY)
				return HIT_DIRTY;
		}
	}
	return MISS;
}

WriteResult writeAddress(Cache cache, unsigned long int address,
	unsigned long int *lru_address, bool *isDirty) {
	_cache *cur_cache = (_cache*)cache;

	unsigned int tag = address >> (cur_cache->blockBits + cur_cache->setBits);
	unsigned int set_index = (address & (FULL_MASK >> cur_cache->tagBits)) >> cur_cache->blockBits;
	_set *cur_set = &(cur_cache->Sets[set_index]);
	_way *cur_way;

	// Check if address is already in cache
	for (int i = 0; i < cur_cache->numOfWays; i++) {
		cur_way = &(cur_set->Ways[i]);
		if (cur_way->tag == tag && cur_way->state != INVALID) { 
			cur_way->state = DIRTY;
			list_del(&(cur_way->LRU));
			list_add_tail(&(cur_way->LRU), &(cur_set->ghost));
			return SUCCESS;
		}
	}

	// Find an invalid address
	for (int j = 0; j < cur_cache->numOfWays; j++) {
		cur_way = &(cur_set->Ways[i]);
		if (cur_way->state == INVALID) {
			cur_way->state = DIRTY;
			list_del(&(cur_way->LRU));
			list_add_tail(&(cur_way->LRU), &(cur_set->ghost));
			return SUCCESS;
		}
	}

	// Find the LRU address
	_way *lru_way = list_entry(&(cur_set->ghost->next), _set, LRU);
	list_del(&(lru_Way->LRU));
	list_add_tail(&(lru_way->LRU), &(cur_set->ghost));
	isDirty = (lru_way->state == DIRTY) ? true : false;
	*lru_address = ((lru_way->tag << cur_cache->setBits) + set_index) << cur_cache->blockBits;
	return REPLACED;
}

WriteResult removeAddress(Cache cache, unsigned long int address,
	bool *isDirty) {
	_cache *cur_cache = (_cache*)cache;
	unsigned int tag = address >> (cur_cache->blockBits + cur_cache->setBits);
	unsigned int set_index = (address & (FULL_MASK >> cur_cache->tagBits)) >> cur_cache->blockBits;
	_set *cur_set = &(cur_cache->Sets[set_index]);
	_way *cur_way;

	for (int i = 0; i < cur_cache->numOfWays; i++) {
		cur_way = &(cur_set->Ways[i]);
		if (cur_way->tag == tag && cur_way->state != INVALID) {
			list_del(&(cur_way->LRU));
			list_add_tail(&(cur_way->LRU), &(cur_set->ghost));
			}
			if (cur_way->state == VALID){
				cur_way->state = INVALID;
				return SUCCESS;
			cur_way->state = INVALID;
			return DIRTY;
		}
	}

	return NOT_FOUND;
}

void ReleaseCache(Cache cache) {
	_cache *cur_cache = (_cache*)cache;
	for (int i = 0; i < Pow2(cur_cache->setBits); i++) {
		for (int j = 0; j < cur_cache->numOfWays; j++) {
			free(&(cur_cache->Sets[j].Ways);
		}
		free(&(cur_cache->Sets[i]))
	}
	free(cur_cache);
}