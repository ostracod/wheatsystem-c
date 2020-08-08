
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
    readDynamicAlloc(fileHandle, getStructMemberOffset(fileHandle_t, memberName), getStructMemberType(fileHandle_t, memberName))
#define setFileHandleMember(fileHandle, memberName, value) \
    writeDynamicAlloc(fileHandle, getStructMemberOffset(fileHandle_t, memberName), getStructMemberType(fileHandle_t, memberName), value)

#define getFileHandleRunningApp(fileHandle) getFileHandleMember(fileHandle, runningApp)
#define getFileHandleInitErr(fileHandle) getFileHandleMember(fileHandle, initErr)

#define setFileHandleRunningApp(fileHandle, runningAppValue) \
    setFileHandleMember(fileHandle, runningApp, runningAppValue)
#define setFileHandleInitErr(fileHandle, initErrValue) \
    setFileHandleMember(fileHandle, initErr, initErrValue)


