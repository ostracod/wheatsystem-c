
///DESC This file provides an implementation of the definitions decribed by `memory/heapMemory`. This implementation uses memory which is natively accessible by the machine's processor.

int8_t heapMemory[HEAP_MEMORY_SIZE];

#define readHeapMemory(address, type) *(type *)(heapMemory + address)
#define writeHeapMemory(address, type, value) (*(type *)(heapMemory + address) = value)


