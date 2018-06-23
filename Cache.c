
#include "Cache.h"
#include "list.h"
#include <math.h>
#include <stdlib.h>

// Defines and stuff

#define ADDRESS_BITS 32
#define FULL_MASK 0xffffffff

typedef struct list_head list_head;

typedef enum {
	BLOCK_INVALID,
	BLOCK_VALID,
	BLOCK_DIRTY
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
	_cache *cache = (_cache*)malloc(sizeof(_cache));
	if (!cache)
		return NULL;
	cache->setBits = PowOfSets;
	cache->blockBits = blockSize;
	cache->tagBits = ADDRESS_BITS - cache->blockBits - cache->setBits;
	cache->numOfWays = Pow2(nWay);
	cache->Sets = (_set*)malloc(sizeof(_set) * NumOfSets);
	if (!cache->Sets) {
		free(cache);
		return NULL;
	}

	// Set + Way Init
	_set *Sets = cache->Sets;
	for (int i = 0; i < NumOfSets; i++) {
		Sets[i].Ways = (_way*)malloc(sizeof(_way) * cache->numOfWays);
		if (!Sets[i].Ways) {
			for (int j = 0; j < i; j++) {
				free(Sets[j].Ways);
			}
			free(Sets);
			free(cache);
			return NULL;
		}
		INIT_LIST_HEAD(&(Sets[i].ghost));
		for (unsigned int j = 0; j < cache->numOfWays; j++) {
			Sets[i].Ways[j].state = BLOCK_INVALID;
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
	for (unsigned int i = 0; i < cur_cache->numOfWays; i++) {
		_way *cur_way = &(cur_set->Ways[i]);
		if (cur_way->tag == tag) {
			if (cur_way->state != BLOCK_INVALID) {
				list_del(&(cur_way->LRU));
				list_add_tail(&(cur_way->LRU), &(cur_set->ghost));
			}
			if (cur_way->state == BLOCK_VALID)
				return HIT;
			if (cur_way->state == BLOCK_DIRTY)
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
	for (unsigned int i = 0; i < cur_cache->numOfWays; i++) {
		cur_way = &(cur_set->Ways[i]);
		if (cur_way->tag == tag && cur_way->state != BLOCK_INVALID) {
			cur_way->state = BLOCK_VALID;
			list_del(&(cur_way->LRU));
			list_add_tail(&(cur_way->LRU), &(cur_set->ghost));
			return SUCCESS;
		}
	}

	// Find an invalid address
	for (unsigned int j = 0; j < cur_cache->numOfWays; j++) {
		cur_way = &(cur_set->Ways[j]);
		if (cur_way->state == BLOCK_INVALID) {
			cur_way->state = BLOCK_VALID;
			cur_way->tag = tag;
			list_del(&(cur_way->LRU));
			list_add_tail(&(cur_way->LRU), &(cur_set->ghost));
			return SUCCESS;
		}
	}

	// Find the LRU address
	_way *lru_way = list_entry(&((cur_set->ghost).next), _way, LRU);
	*isDirty = (lru_way->state == BLOCK_DIRTY) ? true : false;
	*lru_address = ((lru_way->tag << cur_cache->setBits) + set_index) << cur_cache->blockBits;
	lru_way->state = BLOCK_VALID;
	lru_way->tag = tag;
	list_del(&(lru_way->LRU));
	list_add_tail(&(lru_way->LRU), &(cur_set->ghost));
	return REPLACED;
}

WriteResult removeAddress(Cache cache, unsigned long int address,
	bool *isDirty) {
	_cache *cur_cache = (_cache*)cache;
	unsigned int tag = address >> (cur_cache->blockBits + cur_cache->setBits);
	unsigned int set_index = (address & (FULL_MASK >> cur_cache->tagBits)) >> cur_cache->blockBits;
	_set *cur_set = &(cur_cache->Sets[set_index]);
	_way *cur_way;

	for (unsigned int i = 0; i < cur_cache->numOfWays; i++) {
		cur_way = &(cur_set->Ways[i]);
		if (cur_way->tag == tag && cur_way->state != BLOCK_INVALID) {
			list_del(&(cur_way->LRU));
			list_add_tail(&(cur_way->LRU), &(cur_set->ghost));
			}
			if (cur_way->state == BLOCK_VALID){
				cur_way->state = BLOCK_INVALID;
				return SUCCESS;
			cur_way->state = BLOCK_INVALID;
			*isDirty = true;
			return DIRTY;
		}
	}

	return NOT_FOUND;
}

WriteResult setDirty(Cache cache, unsigned long int address){
	_cache *cur_cache = (_cache*)cache;

	unsigned int tag = address >> (cur_cache->blockBits + cur_cache->setBits);
	unsigned int set_index = (address & (FULL_MASK >> cur_cache->tagBits)) >> cur_cache->blockBits;
	_set *cur_set = &(cur_cache->Sets[set_index]);
	_way *cur_way;

	for (unsigned int i = 0; i < cur_cache->numOfWays; i++){
		cur_way = &(cur_set->Ways[i]);
		if (cur_way->tag == tag && cur_way->state != BLOCK_INVALID){
			cur_way->state = BLOCK_DIRTY;
			return SUCCESS;
		}
	}
	return NOT_FOUND;
}

void ReleaseCache(Cache cache) {
	_cache *cur_cache = (_cache*)cache;
	for (int i = 0; i < Pow2(cur_cache->setBits); i++) {
		for (unsigned int j = 0; j < cur_cache->numOfWays; j++) {
			free(&(cur_cache->Sets[j].Ways));
		}
		free(&(cur_cache->Sets[i]));
	}
	free(cur_cache);
}