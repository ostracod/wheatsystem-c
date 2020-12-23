
///DESC This file provides an implementation of the definitions decribed by `fileSystem/fileSystem`. This implementation uses the native Unix file system to store WheatSystem files.

#pragma pack(push, 1)
typedef struct fileHandle {
    int8_t *name;
    int8_t *unixPath;
    int8_t hasAdminPerm;
    int8_t isGuarded;
    int8_t type;
    int32_t contentSize;
    int8_t contentIsDirty;
    int8_t *content;
    allocPointer_t runningApp;
    int8_t initErr;
    int8_t openDepth;
} fileHandle_t;
#pragma pack(pop)

#define getFileHandleMember(fileHandle, memberName) \
    !!!readStructByPointer fileHandle readDynamicAlloc fileHandle_t memberName
#define setFileHandleMember(fileHandle, memberName, value) \
    !!!writeStructByPointer fileHandle writeDynamicAlloc fileHandle_t memberName value

#define getFileHandleType(fileHandle) getFileHandleMember(fileHandle, type)
#define getFileHandleRunningApp(fileHandle) getFileHandleMember(fileHandle, runningApp)
#define getFileHandleInitErr(fileHandle) getFileHandleMember(fileHandle, initErr)

#define setFileHandleRunningApp(fileHandle, runningAppValue) \
    setFileHandleMember(fileHandle, runningApp, runningAppValue)
#define setFileHandleInitErr(fileHandle, initErrValue) \
    setFileHandleMember(fileHandle, initErr, initErrValue)

#define readFile(fileHandle, pos, type) \
    *(type *)(getFileHandleMember(fileHandle, content) + pos)

int8_t initializeFileSystem();
allocPointer_t openFile(heapMemoryOffset_t nameAddress, heapMemoryOffset_t nameSize);
void closeFile(allocPointer_t fileHandle);


