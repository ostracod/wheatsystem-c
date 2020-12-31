
///DESC This file provides data structures which are required by `funcs/memory/linkedListAllocation`.

#pragma pack(push, 1)
typedef struct allocHeader_t {
    int8_t type;
    heapMemoryOffset_t size;
    genericAllocPointer_t next;
} allocHeader_t;
#pragma pack(pop)


