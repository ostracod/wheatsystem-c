
#pragma pack(push, 1)
typedef struct runningAppHeader {
    allocPointer_t fileHandle;
    allocPointer_t localFrame;
    int8_t isWaiting;
} runningAppHeader_t;
#pragma pack(pop)

#define getRunningAppMember(runningApp, memberName) \
    readStructMember(runningApp, readAlloc, runningAppHeader_t, memberName)
#define setRunningAppMember(runningApp, memberName, value) \
    writeStructMember(runningApp, writeAlloc, runningAppHeader_t, memberName, value)

#define getRunningAppFileHandle(runningApp) getRunningAppMember(runningApp, fileHandle)
#define getRunningAppLocalFrame(runningApp) getRunningAppMember(runningApp, localFrame)
#define getRunningAppIsWaiting(runningApp) getRunningAppMember(runningApp, isWaiting)

#define setRunningAppLocalFrame(runningApp, localFrameValue) \
    setRunningAppMember(runningApp, localFrame, localFrameValue)
#define setRunningAppIsWaiting(runningApp, isWaitingValue) \
    setRunningAppMember(runningApp, isWaiting, isWaitingValue)

#define getGlobalFrameAddress(runningApp) \
    (getAllocDataAddress(runningApp) + sizeof(runningAppHeader_t))
#define readGlobalFrame(runningApp, index, type) \
    readHeapMemory(getGlobalFrameAddress(runningApp) + index, type)
#define writeGlobalFrame(runningApp, index, type, value) \
    writeHeapMemory(getGlobalFrameAddress(runningApp) + index, type, value)

void launchApp(allocPointer_t fileHandle);


