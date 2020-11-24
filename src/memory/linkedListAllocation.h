
#pragma pack(push, 1)
typedef struct allocHeader {
    int8_t type;
    heapMemoryOffset_t size;
    allocPointer_t next;
} allocHeader_t;
#pragma pack(pop)

allocPointer_t firstAlloc;

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

allocPointer_t createAlloc(int8_t type, heapMemoryOffset_t size);
int8_t deleteAlloc(allocPointer_t pointer);
void validateAllocPointer(allocPointer_t pointer);


