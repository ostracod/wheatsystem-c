
#include "../../intermediate/headers.h"

#define setBytecodeAppCacheMember(runningApp, memberName, value) \
    writeStructMember(runningApp, writeGlobalFrame, bytecodeAppCache_t, memberName, value)

void initializeBytecodeAppGlobalFrame(allocPointer_t runningApp) {
    allocPointer_t fileHandle = getRunningAppFileHandle(runningApp);
    int32_t functionTableLength = readFile(fileHandle, 4, int32_t);
    int32_t appDataPos = readFile(fileHandle, 8, int32_t);
    setBytecodeAppCacheMember(runningApp, functionTableLength, functionTableLength);
    setBytecodeAppCacheMember(runningApp, appDataPos, appDataPos);
    printf("Function table length: %d\n", functionTableLength);
    printf("App data pos: %d\n", appDataPos);
}


