
#define GENERIC_FILE_TYPE 0
#define BYTECODE_APP_FILE_TYPE 1
#define SYSTEM_APP_FILE_TYPE 2

#define allocIsFileHandle(pointer) \
    (getAllocType(pointer) == DYNAMIC_ALLOC_TYPE && getDynamicAllocMember(pointer, creator) == NULL_ALLOC_POINTER)

#define readFileAndAdvance(fileHandle, pos, type) \
    ({type result = readFile(fileHandle, pos, type); pos += sizeof(type); result;})

allocPointer_t openFileByStringAlloc(allocPointer_t stringAlloc);


