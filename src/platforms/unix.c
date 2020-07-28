
#include "../../intermediate/headers.h"

void printAllPointers() {
    printf("Pointer values:\n");
    int32_t tempPointer = firstAllocation;
    while (tempPointer != NULL_ALLOC_POINTER) {
        printf("%d (Size = %d)\n", tempPointer, getAllocSize(tempPointer));
        tempPointer = getAllocNext(tempPointer);
    }
}

int main(int argc, const char *argv[]) {
    int32_t pointer1 = createAllocation(DYNAMIC_ALLOC_TYPE, 20);
    int32_t pointer2 = createAllocation(DYNAMIC_ALLOC_TYPE, 10);
    createAllocation(DYNAMIC_ALLOC_TYPE, 30);
    printAllPointers();
    deleteAllocation(pointer2);
    printAllPointers();
    createAllocation(DYNAMIC_ALLOC_TYPE, 5);
    printAllPointers();
    createAllocation(DYNAMIC_ALLOC_TYPE, 40);
    printAllPointers();
    deleteAllocation(pointer1);
    printAllPointers();
    return 0;
}


