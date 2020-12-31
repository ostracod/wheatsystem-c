
///DESC This file provides an implementation of the functions decribed by `abstract/fileSystem/fileSystem`. This implementation uses the native Unix file system to store WheatSystem files. This implementation of `abstract/fileSystem/fileSystem` requires the definitions provided by `funcs/communication/unixNativeFile`.

#define getFileHandleMember(fileHandle, memberName) \
    readStructByPointer(castAllocPointer(fileHandle, dynamicAlloc_t), readDynamicAlloc, fileHandle_t, memberName)
#define setFileHandleMember(fileHandle, memberName, value) \
    writeStructByPointer(castAllocPointer(fileHandle, dynamicAlloc_t), writeDynamicAlloc, fileHandle_t, memberName, value)

#define getFileHandleType(fileHandle) getFileHandleMember(fileHandle, type)
#define getFileHandleSize(fileHandle) getFileHandleMember(fileHandle, contentSize)
#define getFileHandleRunningApp(fileHandle) getFileHandleMember(fileHandle, runningApp)
#define getFileHandleInitErr(fileHandle) getFileHandleMember(fileHandle, initErr)

#define setFileHandleRunningApp(fileHandle, runningAppValue) \
    setFileHandleMember(fileHandle, runningApp, runningAppValue)
#define setFileHandleInitErr(fileHandle, initErrValue) \
    setFileHandleMember(fileHandle, initErr, initErrValue)

#define readFile(fileHandle, pos, type) \
    *(type *)(getFileHandleMember(fileHandle, content) + pos)

int8_t initializeFileSystem();
allocPointer_t(fileHandle_t) openFile(heapMemoryOffset_t nameAddress, heapMemoryOffset_t nameSize);
void closeFile(allocPointer_t(fileHandle_t) fileHandle);
allocPointer_t(dynamicAlloc_t) getAllFileNames();


