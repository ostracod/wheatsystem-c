
#define allocIsFileHandle(pointer) \
    (getAllocType(pointer) == DYNAMIC_ALLOC_TYPE && getDynamicAllocCreator(pointer) == NULL_ALLOC_POINTER)

allocPointer_t openFile(heapMemoryOffset_t nameAddress, heapMemoryOffset_t nameSize);
allocPointer_t openFileByStringAlloc(allocPointer_t stringAlloc);
void closeFile(allocPointer_t fileHandle);


