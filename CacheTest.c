#include "Cache.h"
#include <stdbool.h>


bool TestTryAccess(){
    // Create Cache
    Cache L1 = CreateCache(10,2,4);


    // Fill Some lines so we can test
    unsigned int address = 0;
    unsigned int lru;
    bool isDirty;
    WriteResult result;
    for (int i = 0; i < 100; i++){
       result = writeAddress(L1, address, &lru, &isDirty);
       printf("0x%x - %c",address, (result == SUCCESS) ? "SUCCESS" : "REPLACED");
       address++;
    }
    return true;
}

bool TestwriteAddress(){
    return true;
}

bool TestremoveAddress(){
    return true;
}

int main(){
    
    if (!TestTryAccess())
        printf("TryAccess Failed!\n");
        
    if (!TestwriteAddress())
        printf("writeAccess Failed!\n");

    if (!TestremoveAddress())
        printf("TryAccess Failed!\n");

    return 0;
}