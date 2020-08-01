
#include "../../intermediate/headers.h"

void printAllPointers() {
    printf("Pointer values:\n");
    allocPointer_t tempPointer = getFirstAlloc();
    while (tempPointer != NULL_ALLOC_POINTER) {
        printf("%d (Size = %d)\n", tempPointer, getAllocSize(tempPointer));
        tempPointer = getAllocNext(tempPointer);
    }
}

int main(int argc, const char *argv[]) {
    allocPointer_t pointer1 = createAlloc(DYNAMIC_ALLOC_TYPE, 20);
    allocPointer_t pointer2 = createAlloc(DYNAMIC_ALLOC_TYPE, 10);
    createAlloc(DYNAMIC_ALLOC_TYPE, 30);
    printAllPointers();
    deleteAlloc(pointer2);
    printAllPointers();
    createAlloc(DYNAMIC_ALLOC_TYPE, 40);
    printAllPointers();
    createAlloc(DYNAMIC_ALLOC_TYPE, 5);
    printAllPointers();
    deleteAlloc(pointer1);
    printAllPointers();
    return 0;
}


