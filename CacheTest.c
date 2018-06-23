#include "Cache.h"
#include <stdbool.h>
#include <stdio.h>

bool TestCreateCache(){

    Cache l1 = CreateCache(10,4,4);
    Cache l2 = CreateCache(15,3,2);
    Cache l3 = CreateCache(10,4,2);

    ReleaseCache(l1);
    ReleaseCache(l2);
    ReleaseCache(l3);

    return true;
}

bool TestTryAccess(){
    // Create Cache
    unsigned int size = 10;
    unsigned int blockSize = 4;
    unsigned int nWay = 2;

    Cache L1 = CreateCache(size,blockSize,nWay);

    int NumOfBlocks = size - blockSize; // Power of two
	int PowOfSets = NumOfBlocks - nWay; // Power of two


    // Fill Some lines so we can test
    unsigned long int address;
    unsigned int tag = 0;
    unsigned long int lru;
    bool isDirty;
    WriteResult result;
    AccessResult result2;
    for (int i = 0; i < 4; i++){
       address = tag << (PowOfSets + blockSize);
       result = writeAddress(L1, address, &lru, &isDirty);
       printf("Writing to %ld - %s\n",address, (result == SUCCESS) ? "SUCCESS" : "REPLACED");
       tag++;
    }

    tag = 0;
    for (int i = 0; i < 8; i++){
        address = (tag << (PowOfSets + blockSize)) + 0x000000af;
        printf("Reading address %ld - ", address);
        result2 = TryAccess(L1,address);
        printf("%s\n", (result2 == HIT) ? "HIT" : "MISS");
        tag++;            
    }

    tag = 0;
    printf("Now setting all 4 addresses to dirty...\n");
    for (int i = 0; i < 8; i++){
        address = tag << (PowOfSets + blockSize);
        setDirty(L1, address);
        tag++;
    }

    printf("Checking if dirty bit is on...\n");

    tag = 0;
    for (int i = 0; i < 8; i++){
        address = (tag << (PowOfSets + blockSize)) + 0x000000af;
        printf("Reading address %ld - ", address);
        result2 = TryAccess(L1,address);
        printf("%s\n", (result2 == HIT_DIRTY) ? "HIT_DIRTY" : "MISS");
        tag++;            
    }


    return true;
}

bool TestwriteAddress(){
    unsigned int size = 6;
    unsigned int blockSize = 3;
    unsigned int nWay = 0;
    unsigned int set = 0x0000000f;

    Cache L1 = CreateCache(size,blockSize,nWay);

    int NumOfBlocks = size - blockSize; // Power of two
	int PowOfSets = NumOfBlocks - nWay; // Power of two

    WriteResult result;
    unsigned long int address;
    unsigned int tag = 0;
    unsigned long int lru;
    bool isDirty;
    for (int i = 0; i < 8; i++){
        address = (tag << (PowOfSets + blockSize)) + set;
        printf("Writing to Address %ld\n", address);
        result = writeAddress(L1, address, &lru, &isDirty);
        setDirty(L1,address);
        printf("Result is - %s\n", (result == SUCCESS) ? "SUCCESS" : "REPLACED");
        if (result == REPLACED)
            printf("Replaced address is %ld\n", lru);
        tag++;        
        set*=2;
    }

    printf("\n\n\n");
    printf("Testing Shay bug...\n");
    printf("Writing to %d...\n",0x0);
    result = writeAddress(L1, 0x0, &lru, &isDirty);
    printf("Result is %s\n", (result == SUCCESS) ? "SUCCESS" : "REPLACED");
    printf("Writing to %d...\n", 0x00100000);
    result = writeAddress(L1, 0x00100000, &lru, &isDirty);
    printf("Result is %s\n", (result == SUCCESS) ? "SUCCESS" : "REPLACED");
    printf("\n\n\n");

    printf("\n\n");
    tag = 0;

    for(int j = 0; j < 4; j++){
        address = (tag << (PowOfSets + blockSize)) + 0x000000af;
        printf("Writing to address %ld twice...\n", address);
        result = writeAddress(L1, address, &lru, &isDirty);
        printf("Result is - %s\n", (result == SUCCESS) ? "SUCCESS" : "REPLACED");
        result = writeAddress(L1, address, &lru, &isDirty);
        printf("Result is - %s\n", (result == SUCCESS) ? "SUCCESS" : "REPLACED");
        tag++;
    }

    printf("Now setting all 4 addresses to dirty...\n");
    for (int i = 0; i < 8; i++){
        address = (tag << (PowOfSets + blockSize)) + 0x000000af;
        setDirty(L1, address);
        tag++;
    }

    printf("Checking if dirty bit is on...\n");

    tag = 0;
    for (int i = 0; i < 8; i++){
        address = (tag << (PowOfSets + blockSize)) + 0x000000af;
        printf("Writing to address %ld - ", address);
        result = writeAddress(L1,address, &lru, &isDirty);
        printf("%s\n", (isDirty) ? "DIRTY" : "NOT DIRTY");
        tag++;            
    }


    ReleaseCache(L1);

    return true;
}

bool TestremoveAddress(){
    return true;
}

int main(){

    //TestCreateCache();
    //TestTryAccess();
    TestwriteAddress();
    //TestremoveAddress();

    return 0;
}