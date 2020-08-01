
#include "../../intermediate/headers.h"

#define setAllocType(pointer, type) \
    writeHeapMemory(convertPointerToAddress(pointer) + ALLOC_TYPE_OFFSET, int8_t, type)
#define setAllocSize(pointer, size) \
    writeHeapMemory(convertPointerToAddress(pointer) + ALLOC_SIZE_OFFSET, heapMemoryOffset_t, size)
#define setAllocNext(pointer, nextPointer) \
    writeHeapMemory(convertPointerToAddress(pointer) + ALLOC_NEXT_OFFSET, allocPointer_t, nextPointer)

allocPointer_t firstAlloc = NULL_ALLOC_POINTER;

allocPointer_t createAlloc(int8_t type, heapMemoryOffset_t size) {
    
    heapMemoryOffset_t sizeWithHeader = size + ALLOC_DATA_OFFSET;
    heapMemoryOffset_t startAddress = 0;
    allocPointer_t previousPointer = NULL_ALLOC_POINTER;
    allocPointer_t nextPointer = firstAlloc;
    
    // Find a gap which is large enough for the new allocation.
    while (nextPointer != NULL_ALLOC_POINTER) {
        heapMemoryOffset_t endAddress = convertPointerToAddress(nextPointer);
        if (endAddress - startAddress >= sizeWithHeader) {
            break;
        }
        startAddress = endAddress + ALLOC_DATA_OFFSET + getAllocSize(nextPointer);
        previousPointer = nextPointer;
        nextPointer = getAllocNext(nextPointer);
    }
    
    // Return null if there is not enough free memory.
    if (startAddress + sizeWithHeader > HEAP_MEMORY_SIZE) {
        return NULL_ALLOC_POINTER;
    }
    
    // Set up output allocation.
    allocPointer_t output = convertAddressToPointer(startAddress);
    setAllocType(output, type);
    setAllocSize(output, size);
    setAllocNext(output, nextPointer);
    
    // Update previous allocation or firstAlloc.
    if (previousPointer == NULL_ALLOC_POINTER) {
        firstAlloc = output;
    } else {
        setAllocNext(previousPointer, output);
    }
    
    return output;
}

int8_t deleteAlloc(allocPointer_t pointer) {
    
    allocPointer_t previousPointer = NULL_ALLOC_POINTER;
    allocPointer_t nextPointer = firstAlloc;
    
    // Find previous and next allocations.
    while (1) {
        if (nextPointer == NULL_ALLOC_POINTER) {
            return 0;
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
        setAllocNext(previousPointer, nextPointer);
    }
    
    return 1;
}


