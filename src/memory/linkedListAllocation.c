
#include "./headers.h"

#define setAllocMember(pointer, memberName, value) \
    writeHeapMemory(getAllocMemberAddress(pointer, memberName), getStructMemberType(allocHeader_t, memberName), value)

allocPointer_t firstAlloc = NULL_ALLOC_POINTER;

allocPointer_t createAlloc(int8_t type, heapMemoryOffset_t size) {
    
    heapMemoryOffset_t sizeWithHeader = sizeof(allocHeader_t) + size;
    heapMemoryOffset_t startAddress = 0;
    allocPointer_t previousPointer = NULL_ALLOC_POINTER;
    allocPointer_t nextPointer = firstAlloc;
    
    // Find a gap which is large enough for the new allocation.
    while (nextPointer != NULL_ALLOC_POINTER) {
        heapMemoryOffset_t endAddress = convertPointerToAddress(nextPointer);
        if (endAddress - startAddress >= sizeWithHeader) {
            break;
        }
        startAddress = endAddress + sizeof(allocHeader_t) + getAllocSize(nextPointer);
        previousPointer = nextPointer;
        nextPointer = getAllocNext(nextPointer);
    }
    
    // Return null if there is not enough free memory.
    if (startAddress + sizeWithHeader > HEAP_MEMORY_SIZE) {
        return NULL_ALLOC_POINTER;
    }
    
    // Set up output allocation.
    allocPointer_t output = convertAddressToPointer(startAddress);
    setAllocMember(output, type, type);
    setAllocMember(output, size, size);
    setAllocMember(output, next, nextPointer);
    
    // Update previous allocation or firstAlloc.
    if (previousPointer == NULL_ALLOC_POINTER) {
        firstAlloc = output;
    } else {
        setAllocMember(previousPointer, next, output);
    }
    
    return output;
}

int8_t deleteAlloc(allocPointer_t pointer) {
    
    allocPointer_t previousPointer = NULL_ALLOC_POINTER;
    allocPointer_t nextPointer = firstAlloc;
    
    // Find previous and next allocations.
    while (true) {
        if (nextPointer == NULL_ALLOC_POINTER) {
            return false;
        }
        allocPointer_t tempPointer = nextPointer;
        nextPointer = getAllocNext(nextPointer);
        if (tempPointer == pointer) {
            break;
        }
        previousPointer = tempPointer;
    }
    
    // Update previous allocation or firstAlloc to
    // point to the next allocation.
    if (previousPointer == NULL_ALLOC_POINTER) {
        firstAlloc = nextPointer;
    } else {
        setAllocMember(previousPointer, next, nextPointer);
    }
    
    return true;
}


