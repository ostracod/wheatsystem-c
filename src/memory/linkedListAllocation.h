
///DESC This file provides an implementation of the definitions decribed by `memory/allocation`. This implementation uses a linked list to store heap allocations.

#pragma pack(push, 1)
typedef struct allocHeader_t {
    int8_t type;
    heapMemoryOffset_t size;
    genericAllocPointer_t next;
} allocHeader_t;
#pragma pack(pop)

genericAllocPointer_t firstAlloc;

// We need the zero pointer to be null, so we offset
// all addresses by one.
#define convertPointerToAddress(pointer) (pointer - 1)
#define convertAddressToPointer(address) (address + 1)

#define getFirstAlloc() firstAlloc

#define getAllocMemberAddress(pointer, memberName) \
    convertPointerToAddress(pointer) + getStructMemberOffset(allocHeader_t, memberName)
#define getAllocMember(pointer, memberName) \
    readHeapMemory(getAllocMemberAddress(pointer, memberName), getStructMemberType(allocHeader_t, memberName))

#define getAllocDataAddress(pointer) \
    (convertPointerToAddress(pointer) + sizeof(allocHeader_t))
#define readAlloc(pointer, index, type) \
    readHeapMemory(getAllocDataAddress(pointer) + index, type)
#define writeAlloc(pointer, index, type, value) \
    writeHeapMemory(getAllocDataAddress(pointer) + index, type, value)

#define getAllocType(pointer) getAllocMember(pointer, type)
#define getAllocSize(pointer) getAllocMember(pointer, size)
#define getAllocNext(pointer) getAllocMember(pointer, next)

genericAllocPointer_t createAlloc(int8_t type, heapMemoryOffset_t size);
int8_t deleteAlloc(genericAllocPointer_t pointer);
void validateAllocPointer(genericAllocPointer_t pointer);


