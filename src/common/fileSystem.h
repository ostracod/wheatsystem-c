
#define allocIsFileHandle(pointer) \
    (getAllocType(pointer) == DYNAMIC_ALLOC_TYPE && getDynamicAllocCreator(pointer) == NULL_ALLOC_POINTER)

allocPointer_t openFile(allocPointer_t name);
void closeFile(allocPointer_t fileHandle);


