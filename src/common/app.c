
#include "../../intermediate/headers.h"

heapMemoryOffset_t getAppGlobalFrameSize(allocPointer_t fileHandle) {
    int8_t tempType = getFileHandleType(fileHandle);
    if (tempType == BYTECODE_APP_FILE_TYPE) {
        return (heapMemoryOffset_t)readFile(fileHandle, 0, int32_t);
    } else {
        // TODO: Determine size of global frame for system application.
        
        return 0;
    }
}

void launchApp(allocPointer_t fileHandle) {
    allocPointer_t runningApp = getFileHandleRunningApp(fileHandle);
    if (runningApp != NULL_ALLOC_POINTER) {
        return;
    }
    int8_t tempType = getFileHandleType(fileHandle);
    if (tempType != BYTECODE_APP_FILE_TYPE && tempType != SYSTEM_APP_FILE_TYPE) {
        // TODO: Throw an error.
        return;
    }
    runningApp = createAlloc(RUNNING_APP_ALLOC_TYPE, sizeof(runningApp_t));
    heapMemoryOffset_t globalFrameSize = getAppGlobalFrameSize(fileHandle);
    allocPointer_t globalFrame = createAlloc(GLOBAL_FRAME_ALLOC_TYPE, globalFrameSize);
    setRunningAppMember(runningApp, fileHandle, fileHandle);
    setRunningAppMember(runningApp, globalFrame, globalFrame);
    setRunningAppLocalFrame(runningApp, NULL_ALLOC_POINTER);
    setRunningAppIsWaiting(runningApp, false);
    setFileHandleRunningApp(fileHandle, runningApp);
    // TODO: Invoke init function if available.
    
    printf("Running app pointer: %d\n", runningApp);
    printf("Global frame size: %d\n", globalFrameSize);
}


