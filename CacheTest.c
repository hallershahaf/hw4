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
    Cache L1 = CreateCache(10,2,4);


    // Fill Some lines so we can test
    unsigned long int address = 0;
    unsigned long int lru;
    bool isDirty;
    WriteResult result;
    for (int i = 0; i < 100; i++){
       result = writeAddress(L1, address, &lru, &isDirty);
       printf("%ld - %s\n",address, (result == SUCCESS) ? "SUCCESS" : "REPLACED");
       address++;
    }
    return true;
}

bool TestwriteAddress(){
    unsigned int size = 10;
    unsigned int blockSize = 4;
    unsigned int nWay = 2;

    Cache L1 = CreateCache(size,blockSize,nWay);

    int NumOfBlocks = size - blockSize; // Power of two
	int PowOfSets = NumOfBlocks - nWay; // Power of two

    WriteResult result;
    unsigned long int address;
    unsigned int tag = 0;
    unsigned long int lru;
    bool isDirty;
    for (int i = 0; i < 20; i++){
        address = tag << (PowOfSets + blockSize);
        printf("Writing to Address %ld\n", address);
        result = writeAddress(L1, address, &lru, &isDirty);
        setDirty(L1,address);
        printf("Result is - %s\n", (result == SUCCESS) ? "SUCCESS" : "REPLACED");
        tag++;        
    }

    ReleaseCache(L1);

    return true;
}

bool TestremoveAddress(){
    return true;
}

int main(){
    // if (!TestCreateCache()){
    //     printf("TryAccess Failed!\n");
    // }
    
    // if (!TestTryAccess())
    //     printf("TryAccess Failed!\n");
        
    if (!TestwriteAddress())
        printf("writeAccess Failed!\n");

    // if (!TestremoveAddress())
    //     printf("TryAccess Failed!\n");

    return 0;
}