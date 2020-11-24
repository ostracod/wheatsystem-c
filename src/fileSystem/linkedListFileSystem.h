
#pragma pack(push, 1)
typedef struct fileHeader {
    uint8_t attributes;
    uint8_t nameSize;
    int32_t contentSize;
    int32_t next;
} fileHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fileHandle {
    int32_t address;
    uint8_t attributes;
    uint8_t nameSize;
    int32_t contentSize;
    allocPointer_t runningApp;
    int8_t initErr;
    int8_t openDepth;
} fileHandle_t;
#pragma pack(pop)

#define getFileHeaderMember(address, memberName) \
    !!!readStructByAddress address readStorageSpace fileHeader_t memberName
#define setFileHeaderMember(address, memberName, value) \
    !!!writeStructByAddress address writeStorageSpace fileHeader_t memberName value

#define getFileHandleMember(fileHandle, memberName) \
    !!!readStructByPointer fileHandle readDynamicAlloc fileHandle_t memberName
#define setFileHandleMember(fileHandle, memberName, value) \
    !!!writeStructByPointer fileHandle writeDynamicAlloc fileHandle_t memberName value

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
    !!!readValueByPointer fileHandle readFileRange pos type

allocPointer_t openFile(heapMemoryOffset_t nameAddress, heapMemoryOffset_t nameSize);
void closeFile(allocPointer_t fileHandle);
void readFileRange(
    void *destination,
    allocPointer_t fileHandle,
    int32_t pos,
    int32_t amount
);


