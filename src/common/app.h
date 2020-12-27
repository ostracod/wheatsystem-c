
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
typedef struct runningAppHeader_t {
    allocPointer_t(fileHandle_t) fileHandle;
    allocPointer_t(localFrame_t) localFrame;
    int8_t isWaiting;
} runningAppHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct runningApp_t {
    runningAppHeader_t header;
    int8_t globalFrameData[0];
} runningApp_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct localFrameHeader_t {
    allocPointer_t(runningApp_t) implementer;
    int32_t functionIndex;
    allocPointer_t(localFrame_t) previousLocalFrame;
    allocPointer_t(argFrame_t) nextArgFrame;
    int8_t lastErrorCode;
} localFrameHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct localFrame_t {
    localFrameHeader_t header;
    int8_t data[0];
} localFrame_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct argFrame_t {
    int8_t data[0];
} argFrame_t;
#pragma pack(pop)

#define getRunningAppMember(runningApp, memberName) \
    readStructByPointer(runningApp.genericPointer, readAlloc, runningAppHeader_t, memberName)
#define setRunningAppMember(runningApp, memberName, value) \
    writeStructByPointer(runningApp.genericPointer, writeAlloc, runningAppHeader_t, memberName, value)

#define getGlobalFrameDataAddress(runningApp) \
    (getAllocDataAddress(runningApp.genericPointer) + sizeof(runningAppHeader_t))
#define getGlobalFrameSize(runningApp) \
    (getAllocSize(runningApp.genericPointer) - sizeof(runningAppHeader_t))

#define readGlobalFrame(runningApp, index, type) \
    readHeapMemory(getGlobalFrameDataAddress(runningApp) + index, type)
#define writeGlobalFrame(runningApp, index, type, value) \
    writeHeapMemory(getGlobalFrameDataAddress(runningApp) + index, type, value)

#define getLocalFrameMember(localFrame, memberName) \
    readStructByPointer(localFrame.genericPointer, readAlloc, localFrameHeader_t, memberName)
#define setLocalFrameMember(localFrame, memberName, value) \
    writeStructByPointer(localFrame.genericPointer, writeAlloc, localFrameHeader_t, memberName, value)

#define getLocalFrameDataAddress(localFrame) \
    (getAllocDataAddress(localFrame.genericPointer) + sizeof(localFrameHeader_t))
#define getLocalFrameSize(localFrame) \
    (getAllocSize(localFrame.genericPointer) - sizeof(localFrameHeader_t))

#define readLocalFrame(localFrame, index, type) \
    readHeapMemory(getLocalFrameDataAddress(localFrame) + index, type)
#define writeLocalFrame(localFrame, index, type, value) \
    writeHeapMemory(getLocalFrameDataAddress(localFrame) + index, type, value)

#define getArgFrameSize(argFrame) getAllocSize(argFrame.genericPointer)
#define getArgFrameDataAddress(argFrame) getAllocDataAddress(argFrame.genericPointer)

#define readArgFrame(argFrame, index, type) \
    readAlloc(argFrame.genericPointer, index, type)
#define writeArgFrame(argFrame, index, type, value) \
    writeAlloc(argFrame.genericPointer, index, type, value)

#define getPreviousArgFrame() ({ \
    allocPointer_t(localFrame_t) tempLocalFrame = getLocalFrameMember(currentLocalFrame, previousLocalFrame); \
    getLocalFrameMember(tempLocalFrame, nextArgFrame); \
})
#define cleanUpNextArgFrame() cleanUpNextArgFrameHelper(currentLocalFrame)

allocPointer_t(runningApp_t) currentThreadApp;
allocPointer_t(localFrame_t) currentLocalFrame;
allocPointer_t(runningApp_t) currentImplementer;
allocPointer_t(fileHandle_t) currentImplementerFileHandle;

int32_t findFunctionById(allocPointer_t(runningApp_t) runningApp, int32_t functionId);
allocPointer_t(runningApp_t) getCurrentCaller();
allocPointer_t(argFrame_t) createNextArgFrame(heapMemoryOffset_t size);
void cleanUpNextArgFrameHelper(allocPointer_t(localFrame_t) localFrame);
void launchApp(allocPointer_t(fileHandle_t) fileHandle);
void callFunction(
    allocPointer_t(runningApp_t) threadApp,
    allocPointer_t(runningApp_t) implementer,
    int32_t functionIndex
);
void returnFromFunction();
void scheduleAppThread(allocPointer_t(runningApp_t) runningApp);
void runAppSystem();


