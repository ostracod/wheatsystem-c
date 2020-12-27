
#define GENERIC_FILE_TYPE 0
#define BYTECODE_APP_FILE_TYPE 1
#define SYSTEM_APP_FILE_TYPE 2

#define getHasAdminPermFromFileAttributes(fileAttributes) ((fileAttributes & 0x08) > 0)
#define getIsGuardedFromFileAttributes(fileAttributes) ((fileAttributes & 0x04) > 0)
#define getTypeFromFileAttributes(fileAttributes) (fileAttributes & 0x03)

#define allocIsFileHandle(pointer) \
    (getAllocType(pointer) == DYNAMIC_ALLOC_TYPE && pointerIsNull(getDynamicAllocMember(castGenericPointer(pointer, dynamicAlloc_t), creator)))

allocPointer_t(fileHandle_t) openFileByStringAlloc(
    allocPointer_t(stringAlloc_t) stringAlloc
);
allocPointer_t(fileHandle_t) openFileByDynamicStringAlloc(
    allocPointer_t(dynamicAlloc_t) dynamicStringAlloc
);
void validateFileHandle(allocPointer_t(fileHandle_t) fileHandle);


