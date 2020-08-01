
#define ALLOC_TYPE_OFFSET 0
#define ALLOC_SIZE_OFFSET (ALLOC_TYPE_OFFSET + 1)
#define ALLOC_NEXT_OFFSET (ALLOC_SIZE_OFFSET + sizeof(heapMemoryOffset_t))
#define ALLOC_DATA_OFFSET (ALLOC_NEXT_OFFSET + sizeof(allocPointer_t))

allocPointer_t firstAlloc;

// We need the zero pointer to be null, so we offset
// all addresses by one.
#define convertPointerToAddress(pointer) (pointer - 1)
#define convertAddressToPointer(address) (address + 1)

#define getFirstAlloc() firstAlloc

#define getAllocType(pointer) \
    readHeapMemory(convertPointerToAddress(pointer) + ALLOC_TYPE_OFFSET, int8_t)
#define getAllocSize(pointer) \
    readHeapMemory(convertPointerToAddress(pointer) + ALLOC_SIZE_OFFSET, heapMemoryOffset_t)
#define getAllocNext(pointer) \
    readHeapMemory(convertPointerToAddress(pointer) + ALLOC_NEXT_OFFSET, allocPointer_t)

#define readAlloc(pointer, index, type) \
    readHeapMemory(convertPointerToAddress(pointer) + ALLOC_DATA_OFFSET + index, type)
#define writeAlloc(pointer, index, type, value) \
    writeHeapMemory(convertPointerToAddress(pointer) + ALLOC_DATA_OFFSET + index, type, value)


