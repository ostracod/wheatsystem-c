
///DESC This file provides an implementation of the definitions decribed by `fileSystem/fileSystem`. This implementation uses a linked list to store files. This implemenation of `fileSystem/fileSystem` requires an implemenation of `fileSystem/storageSpace`.

#pragma pack(push, 1)
typedef struct fileHeader_t {
    uint8_t attributes;
    uint8_t nameSize;
    int32_t contentSize;
    int32_t next;
} fileHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fileHandle_t {
    int32_t address;
    uint8_t attributes;
    uint8_t nameSize;
    int32_t contentSize;
    allocPointer_t(runningApp_t) runningApp;
    int8_t initErr;
    int8_t openDepth;
} fileHandle_t;
#pragma pack(pop)

#define getFileHeaderMember(address, memberName) \
    readStructByAddress(address, readStorageSpace, fileHeader_t, memberName)
#define setFileHeaderMember(address, memberName, value) \
    writeStructByAddress(address, writeStorageSpace, fileHeader_t, memberName, value)

#define getFileHandleMember(fileHandle, memberName) \
    readStructByPointer(castAllocPointer(fileHandle, dynamicAlloc_t), readDynamicAlloc, fileHandle_t, memberName)
#define setFileHandleMember(fileHandle, memberName, value) \
    writeStructByPointer(castAllocPointer(fileHandle, dynamicAlloc_t), writeDynamicAlloc, fileHandle_t, memberName, value)

#define initializeFileSystem() initializeStorageSpace()

#define getFileHandleType(fileHandle) \
    getTypeFromFileAttributes(getFileHandleMember(fileHandle, attributes))
#define getFileHandleSize(fileHandle) getFileHandleMember(fileHandle, contentSize)
#define getFileHandleRunningApp(fileHandle) getFileHandleMember(fileHandle, runningApp)
#define getFileHandleInitErr(fileHandle) getFileHandleMember(fileHandle, initErr)

#define setFileHandleRunningApp(fileHandle, runningAppValue) \
    setFileHandleMember(fileHandle, runningApp, runningAppValue)
#define setFileHandleInitErr(fileHandle, initErrValue) \
    setFileHandleMember(fileHandle, initErr, initErrValue)

#define readFile(fileHandle, pos, type) \
    readValueByPointer(fileHandle, readFileRange, pos, type)

allocPointer_t(fileHandle_t) openFile(heapMemoryOffset_t nameAddress, heapMemoryOffset_t nameSize);
void closeFile(allocPointer_t(fileHandle_t) fileHandle);
void readFileRange(
    void *destination,
    allocPointer_t(fileHandle_t) fileHandle,
    int32_t pos,
    int32_t amount
);
allocPointer_t(dynamicAlloc_t) getAllFileNames();


