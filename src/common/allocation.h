
#define BYTECODE_APP_ALLOC_TYPE 0
#define SYSTEM_APP_ALLOC_TYPE 1
#define GLOBAL_FRAME_ALLOC_TYPE 2
#define LOCAL_FRAME_ALLOC_TYPE 3
#define ARG_FRAME_ALLOC_TYPE 4
#define DYNAMIC_ALLOC_TYPE 5
#define STRING_ALLOC_TYPE 6

#define NULL_ALLOC_POINTER 0

#define DYNAMIC_ALLOC_IS_GUARDED_OFFSET 0
#define DYNAMIC_ALLOC_CREATOR_OFFSET (DYNAMIC_ALLOC_IS_GUARDED_OFFSET + 1)
#define DYNAMIC_ALLOC_DATA_OFFSET (DYNAMIC_ALLOC_CREATOR_OFFSET + sizeof(allocPointer_t))

#define getDynamicAllocSize(pointer) \
    (getAllocSize(pointer) - DYNAMIC_ALLOC_DATA_OFFSET)
#define getDynamicAllocIsGuarded(pointer) \
    readAlloc(pointer, DYNAMIC_ALLOC_IS_GUARDED_OFFSET, int8_t)
#define getDynamicAllocCreator(pointer) \
    readAlloc(pointer, DYNAMIC_ALLOC_CREATOR_OFFSET, allocPointer_t)

#define readDynamicAlloc(pointer, index, type) \
    readAlloc(pointer, DYNAMIC_ALLOC_DATA_OFFSET + index, type)
#define writeDynamicAlloc(pointer, index, type, value) \
    writeAlloc(pointer, DYNAMIC_ALLOC_DATA_OFFSET + index, type, value)

#define createAllocFromStringConstant(stringConstant) \
    createAllocFromStringConstantHelper(stringConstant, getStringConstantSize(stringConstant))

allocPointer_t createAlloc(int8_t type, heapMemoryOffset_t size);
int8_t deleteAlloc(allocPointer_t pointer);
allocPointer_t createDynamicAlloc(
    heapMemoryOffset_t size,
    int8_t isGuarded,
    allocPointer_t creator
);
allocPointer_t createAllocFromStringConstantHelper(
    stringConstant_t stringConstant,
    heapMemoryOffset_t size
);


