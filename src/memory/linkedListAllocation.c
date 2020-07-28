
#include "../../intermediate/headers.h"

#define setAllocType(pointer, type) \
    writeHeapMemoryInt8(convertPointerToAddress(pointer) + ALLOC_TYPE_OFFSET, type)
#define setAllocSize(pointer, size) \
    writeHeapMemoryInt32(convertPointerToAddress(pointer) + ALLOC_SIZE_OFFSET, size)
#define setAllocNext(pointer, nextPointer) \
    writeHeapMemoryInt32(convertPointerToAddress(pointer) + ALLOC_NEXT_OFFSET, nextPointer)

int32_t firstAllocation = NULL_ALLOC_POINTER;

int32_t createAllocation(int8_t type, int32_t size) {
    
    int32_t sizeWithHeader = size + ALLOC_DATA_OFFSET;
    int32_t previousPointer = NULL_ALLOC_POINTER;
    int32_t startAddress = 0;
    int32_t nextPointer = firstAllocation;
    
    // Find a gap which is large enough for the new allocation.
    while (nextPointer != NULL_ALLOC_POINTER) {
        int32_t endAddress = convertPointerToAddress(nextPointer);
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
    int32_t output = convertAddressToPointer(startAddress);
    setAllocType(output, type);
    setAllocSize(output, size);
    setAllocNext(output, nextPointer);
    
    // Update previous allocation or firstAllocation.
    if (previousPointer == NULL_ALLOC_POINTER) {
        firstAllocation = output;
    } else {
        setAllocNext(previousPointer, output);
    }
    
    return output;
}

int8_t deleteAllocation(int32_t pointer) {
    
    int32_t previousPointer = NULL_ALLOC_POINTER;
    int32_t nextPointer = firstAllocation;
    
    // Find previous and next allocations.
    while (1) {
        if (nextPointer == NULL_ALLOC_POINTER) {
            return 0;
        }
        int32_t tempPointer = nextPointer;
        nextPointer = getAllocNext(nextPointer);
        if (tempPointer == pointer) {
            break;
        }
        previousPointer = tempPointer;
    }
    
    // Update previous allocation or firstAllocation to
    // point to the next allocation.
    if (previousPointer == NULL_ALLOC_POINTER) {
        firstAllocation = nextPointer;
    } else {
        setAllocNext(previousPointer, nextPointer);
    }
    
    return 1;
}


