
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

#pragma pack(push, 1)
typedef struct localFrameHeader {
    allocPointer_t implementer;
    allocPointer_t caller;
    int32_t functionIndex;
    allocPointer_t previousLocalFrame;
    allocPointer_t nextArgFrame;
    int8_t lastErrorCode;
} localFrameHeader_t;
#pragma pack(pop)

#define getRunningAppMember(runningApp, memberName) \
    !!!readStructByPointer runningApp readAlloc runningAppHeader_t memberName
#define setRunningAppMember(runningApp, memberName, value) \
    !!!writeStructByPointer runningApp writeAlloc runningAppHeader_t memberName value

#define getGlobalFrameDataAddress(runningApp) \
    (getAllocDataAddress(runningApp) + sizeof(runningAppHeader_t))
#define readGlobalFrame(runningApp, index, type) \
    readHeapMemory(getGlobalFrameDataAddress(runningApp) + index, type)
#define writeGlobalFrame(runningApp, index, type, value) \
    writeHeapMemory(getGlobalFrameDataAddress(runningApp) + index, type, value)

#define getLocalFrameMember(localFrame, memberName) \
    !!!readStructByPointer localFrame readAlloc localFrameHeader_t memberName
#define setLocalFrameMember(localFrame, memberName, value) \
    !!!writeStructByPointer localFrame writeAlloc localFrameHeader_t memberName value

#define getLocalFrameDataAddress(localFrame) \
    (getAllocDataAddress(localFrame) + sizeof(localFrameHeader_t))
#define readLocalFrame(localFrame, index, type) \
    readHeapMemory(getLocalFrameDataAddress(localFrame) + index, type)
#define writeLocalFrame(localFrame, index, type, value) \
    writeHeapMemory(getLocalFrameDataAddress(localFrame) + index, type, value)

#define getPreviousArgFrame() ({ \
    allocPointer_t tempLocalFrame = getLocalFrameMember(currentLocalFrame, previousLocalFrame); \
    getLocalFrameMember(tempLocalFrame, nextArgFrame); \
})

allocPointer_t currentThreadApp;
allocPointer_t currentLocalFrame;
allocPointer_t currentImplementer;
allocPointer_t currentImplementerFileHandle;

int32_t findFunctionById(allocPointer_t runningApp, int32_t functionId);
void launchApp(allocPointer_t fileHandle);
void callFunction(
    allocPointer_t threadApp,
    allocPointer_t caller,
    allocPointer_t implementer,
    int32_t functionIndex
);
void cleanUpNextArgFrame();
void returnFromFunction();
void scheduleAppThread(allocPointer_t runningApp);
void runAppSystem();


