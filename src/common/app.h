
#pragma pack(push, 1)
typedef struct runningApp {
    allocPointer_t fileHandle;
    allocPointer_t globalFrame;
    allocPointer_t localFrame;
    int8_t isWaiting;
} runningApp_t;
#pragma pack(pop)

#define getRunningAppMember(runningApp, memberName) \
    readAlloc(runningApp, getStructMemberOffset(runningApp_t, memberName), getStructMemberType(runningApp_t, memberName))
#define setRunningAppMember(runningApp, memberName, value) \
    writeAlloc(runningApp, getStructMemberOffset(runningApp_t, memberName), getStructMemberType(runningApp_t, memberName), value)

#define getRunningAppFileHandle(runningApp) getRunningAppMember(runningApp, fileHandle)
#define getRunningAppGlobalFrame(runningApp) getRunningAppMember(runningApp, globalFrame)
#define getRunningAppLocalFrame(runningApp) getRunningAppMember(runningApp, localFrame)
#define getRunningAppIsWaiting(runningApp) getRunningAppMember(runningApp, isWaiting)

#define setRunningAppLocalFrame(runningApp, localFrameValue) \
    setRunningAppMember(runningApp, localFrame, localFrameValue)
#define setRunningAppIsWaiting(runningApp, isWaitingValue) \
    setRunningAppMember(runningApp, isWaiting, isWaitingValue)

void launchApp(allocPointer_t fileHandle);


