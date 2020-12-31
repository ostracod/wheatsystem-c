
///DESC This file provides an implementation of the functions decribed by `abstract/fileSystem/fileSystem`. This implementation uses a linked list to store files. This implemenation of `abstract/fileSystem/fileSystem` requires an implemenation of `abstract/fileSystem/storageSpace`.

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


