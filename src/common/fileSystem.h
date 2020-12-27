
///CONST
    ///TYPE int8_t
#define GENERIC_FILE_TYPE 0
///CONST
    ///TYPE int8_t
#define BYTECODE_APP_FILE_TYPE 1
///CONST
    ///TYPE int8_t
#define SYSTEM_APP_FILE_TYPE 2

///FUNC
    ///RET int8_t
    ///DESC Retrieves whether a file has admin permission from the given bit field.
    ///ARG fileAttributes
        ///TYPE int8_t
#define getHasAdminPermFromFileAttributes(fileAttributes) ((fileAttributes & 0x08) > 0)
///FUNC
    ///RET int8_t
    ///DESC Retrieves whether a file is guarded from the given bit field.
    ///ARG fileAttributes
        ///TYPE int8_t
#define getIsGuardedFromFileAttributes(fileAttributes) ((fileAttributes & 0x04) > 0)
///FUNC
    ///RET int8_t
    ///DESC Retrieves the type of a file from the given bit field.
    ///ARG fileAttributes
        ///TYPE int8_t
#define getTypeFromFileAttributes(fileAttributes) (fileAttributes & 0x03)

///FUNC
    ///RET int8_t
    ///DESC Retrieves whether the given heap allocation is a file handle.
    ///ARG pointer
        ///TYPE genericAllocPointer_t
#define allocIsFileHandle(pointer) \
    (getAllocType(pointer) == DYNAMIC_ALLOC_TYPE && pointerIsNull(getDynamicAllocMember(castGenericPointer(pointer, dynamicAlloc_t), creator)))

///FUNC
    ///DESC Opens a file with the given name. Uses `openFile` for underlying logic.
    ///ARG stringAlloc
        ///DESC Name of file to open.
allocPointer_t(fileHandle_t) openFileByStringAlloc(
    allocPointer_t(stringAlloc_t) stringAlloc
);
///FUNC
    ///DESC Opens a file with the given name. Uses `openFile` for underlying logic.
    ///ARG dynamicStringAlloc
        ///DESC Name of file to open.
allocPointer_t(fileHandle_t) openFileByDynamicStringAlloc(
    allocPointer_t(dynamicAlloc_t) dynamicStringAlloc
);
///FUNC
    ///DESC Verifies whether the given pointer references a valid file handle. May assign a new value to `unhandledErrorCode`.
void validateFileHandle(allocPointer_t(fileHandle_t) fileHandle);


