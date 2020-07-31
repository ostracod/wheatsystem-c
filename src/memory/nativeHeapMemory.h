
int8_t heapMemory[HEAP_MEMORY_SIZE];

#define readHeapMemory(address, type) *(type *)(heapMemory + address)
#define writeHeapMemory(address, type, value) (*(type *)(heapMemory + address) = value)


