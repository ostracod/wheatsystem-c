
#define RUNNING_APP_ALLOC_TYPE 0
#define GLOBAL_FRAME_ALLOC_TYPE 1
#define LOCAL_FRAME_ALLOC_TYPE 2
#define ARG_FRAME_ALLOC_TYPE 3
#define DYNAMIC_ALLOC_TYPE 4
#define STRING_ALLOC_TYPE 5

#define NULL_ALLOC_POINTER 0

#define DYNAMIC_ALLOC_IS_GUARDED_OFFSET 0
#define DYNAMIC_ALLOC_CREATOR_OFFSET (DYNAMIC_ALLOC_IS_GUARDED_OFFSET + 1)
#define DYNAMIC_ALLOC_DATA_OFFSET (DYNAMIC_ALLOC_CREATOR_OFFSET + sizeof(allocPointer_t))

#pragma pack(push, 1)
typedef struct dynamicAllocHeader {
    int8_t isGuarded;
    allocPointer_t creator;
} dynamicAllocHeader_t;
#pragma pack(pop)

#define getDynamicAllocMember(pointer, memberName) \
    readAlloc(pointer, getStructMemberOffset(dynamicAllocHeader_t, memberName), getStructMemberType(dynamicAllocHeader_t, memberName))

#define getDynamicAllocDataAddress(pointer) \
    (getAllocDataAddress(pointer) + sizeof(dynamicAllocHeader_t))
#define readDynamicAlloc(pointer, index, type) \
    readHeapMemory(getDynamicAllocDataAddress(pointer) + index, type)
#define writeDynamicAlloc(pointer, index, type, value) \
    writeHeapMemory(getDynamicAllocDataAddress(pointer) + index, type, value)

#define getDynamicAllocSize(pointer) \
    (getAllocSize(pointer) - sizeof(dynamicAllocHeader_t))
#define getDynamicAllocIsGuarded(pointer) getDynamicAllocMember(pointer, isGuarded)
#define getDynamicAllocCreator(pointer) getDynamicAllocMember(pointer, creator)

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


