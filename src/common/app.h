
#pragma pack(push, 1)
typedef struct app {
    allocPointer_t fileHandle;
    allocPointer_t globalFrame;
    allocPointer_t localFrame;
    int8_t isWaiting;
} app_t;
#pragma pack(pop)

#define getAppMember(app, memberName) \
    readAlloc(app, getStructMemberOffset(app_t, memberName), getStructMemberType(app_t, memberName))
#define setAppMember(app, memberName, value) \
    writeAlloc(fileHandle, getStructMemberOffset(app_t, memberName), getStructMemberType(app_t, memberName), value)

void launchApp(allocPointer_t fileHandle);


