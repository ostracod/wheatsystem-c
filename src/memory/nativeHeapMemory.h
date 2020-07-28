
int8_t heapMemory[HEAP_MEMORY_SIZE];

#define readHeapMemoryInt8(address) *(heapMemory + address)
#define readHeapMemoryInt32(address) *(int32_t)(heapMemory + address)
#define writeHeapMemoryInt8(address, value) (*(heapMemory + address) = value)
#define writeHeapMemoryInt32(address, value) (*(int32_t)(heapMemory + address) = value)


