
#define INIT_FUNC_ID 1
#define KILL_FUNC_ID 2
#define LISTEN_TERM_FUNC_ID 3
#define TERM_SIZE_FUNC_ID 4
#define WRT_TERM_FUNC_ID 5
#define TERM_INPUT_FUNC_ID 6
#define START_SERIAL_FUNC_ID 7
#define STOP_SERIAL_FUNC_ID 8
#define WRT_SERIAL_FUNC_ID 9
#define SERIAL_INPUT_FUNC_ID 10
#define SET_GPIO_MODE_FUNC_ID 11
#define READ_GPIO_FUNC_ID 12
#define WRT_GPIO_FUNC_ID 13

#pragma pack(push, 1)
typedef struct runningAppHeader {
    allocPointer_t fileHandle;
    allocPointer_t localFrame;
    int8_t isWaiting;
} runningAppHeader_t;
#pragma pack(pop)

#define getRunningAppMember(runningApp, memberName) \
    !!!readStructMember runningApp readAlloc runningAppHeader_t memberName
#define setRunningAppMember(runningApp, memberName, value) \
    !!!writeStructMember runningApp writeAlloc runningAppHeader_t memberName value

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


