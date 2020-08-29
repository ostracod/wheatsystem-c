
// TODO: Fix all of this stuff.

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

#define getFileHandleType(fileHandle) 0
#define getFileHandleRunningApp(fileHandle) 0
#define getFileHandleInitErr(fileHandle) 0

#define setFileHandleRunningApp(fileHandle, runningAppValue)
#define setFileHandleInitErr(fileHandle, initErrValue)

#define readFile(fileHandle, pos, type) 0

allocPointer_t openFile(heapMemoryOffset_t nameAddress, heapMemoryOffset_t nameSize);
void closeFile(allocPointer_t fileHandle);


