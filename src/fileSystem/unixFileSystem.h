
#pragma pack(push, 1)
typedef struct fileHandle {
    int8_t *name;
    int8_t *content;
    allocPointer_t app;
    int8_t initErr;
    int8_t openDepth;
} fileHandle_t;
#pragma pack(pop)

#define getFileHandleMember(fileHandle, memberName) \
    readDynamicAlloc(fileHandle, getStructMemberOffset(fileHandle_t, memberName), getStructMemberType(fileHandle_t, memberName))
#define setFileHandleMember(fileHandle, memberName, value) \
    writeDynamicAlloc(fileHandle, getStructMemberOffset(fileHandle_t, memberName), getStructMemberType(fileHandle_t, memberName), value)

#define getFileHandleApp(fileHandle) getFileHandleMember(fileHandle, app)
#define getFileHandleInitErr(fileHandle) getFileHandleMember(fileHandle, initErr)

#define setFileHandleApp(fileHandle, appValue) \
    setFileHandleMember(fileHandle, app, appValue)
#define setFileHandleInitErr(fileHandle, initErrValue) \
    setFileHandleMember(fileHandle, initErr, initErrValue)


