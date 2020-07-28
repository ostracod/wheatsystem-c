
#define ALLOC_TYPE_OFFSET 0
#define ALLOC_SIZE_OFFSET (ALLOC_TYPE_OFFSET + 1)
#define ALLOC_NEXT_OFFSET (ALLOC_SIZE_OFFSET + 4)
#define ALLOC_DATA_OFFSET (ALLOC_NEXT_OFFSET + 4)

int32_t firstAllocation;

// We need the zero pointer to be null, so we offset
// all addresses by one.
#define convertPointerToAddress(pointer) (pointer - 1)
#define convertAddressToPointer(address) (address + 1)

#define getAllocType(pointer) \
    readHeapMemoryInt8(convertPointerToAddress(pointer) + ALLOC_TYPE_OFFSET)
#define getAllocSize(pointer) \
    readHeapMemoryInt32(convertPointerToAddress(pointer) + ALLOC_SIZE_OFFSET)
#define getAllocNext(pointer) \
    readHeapMemoryInt32(convertPointerToAddress(pointer) + ALLOC_NEXT_OFFSET)

#define readAllocInt8(pointer, index) \
    readHeapMemoryInt8(convertPointerToAddress(pointer) + ALLOC_DATA_OFFSET + index)
#define readAllocInt32(pointer, index) \
    readHeapMemoryInt32(convertPointerToAddress(pointer) + ALLOC_DATA_OFFSET + index)

#define writeAllocInt8(pointer, index, value) \
    writeHeapMemoryInt8(convertPointerToAddress(pointer) + ALLOC_DATA_OFFSET + index, value)
#define writeAllocInt32(pointer, index, value) \
    writeHeapMemoryInt32(convertPointerToAddress(pointer) + ALLOC_DATA_OFFSET + index, value)


