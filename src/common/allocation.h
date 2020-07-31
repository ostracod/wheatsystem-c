
#define BYTECODE_APP_ALLOC_TYPE 0
#define SYSTEM_APP_ALLOC_TYPE 1
#define GLOBAL_FRAME_ALLOC_TYPE 2
#define LOCAL_FRAME_ALLOC_TYPE 3
#define ARG_FRAME_ALLOC_TYPE 4
#define DYNAMIC_ALLOC_TYPE 5

#define NULL_ALLOC_POINTER 0

#define DYNAMIC_ALLOC_IS_GUARDED_OFFSET 0
#define DYNAMIC_ALLOC_CREATOR_OFFSET (DYNAMIC_ALLOC_IS_GUARDED_OFFSET + 1)
#define DYNAMIC_ALLOC_DATA_OFFSET (DYNAMIC_ALLOC_CREATOR_OFFSET + sizeof(allocPointer_t))

allocPointer_t createAllocation(int8_t type, heapMemoryOffset_t size);
int8_t deleteAllocation(allocPointer_t pointer);


