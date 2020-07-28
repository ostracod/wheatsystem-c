
#define BYTECODE_APP_ALLOC_TYPE 0
#define SYSTEM_APP_ALLOC_TYPE 1
#define GLOBAL_FRAME_ALLOC_TYPE 2
#define LOCAL_FRAME_ALLOC_TYPE 3
#define ARG_FRAME_ALLOC_TYPE 4
#define DYNAMIC_ALLOC_TYPE 5

#define NULL_ALLOC_POINTER 0

int32_t createAllocation(int8_t type, int32_t size);
int8_t deleteAllocation(int32_t pointer);


