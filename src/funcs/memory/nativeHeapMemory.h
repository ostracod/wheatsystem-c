
///DESC This file provides an implementation of the functions decribed by `abstract/memory/heapMemory`. This implementation uses memory which is natively accessible by the machine's processor.

#define readHeapMemory(address, type) (*(type *)(heapMemory + address))
#define writeHeapMemory(address, type, value) (*(type *)(heapMemory + address) = value)


