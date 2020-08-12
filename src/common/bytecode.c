
#include "./headers.h"

#define setBytecodeAppCacheMember(runningApp, memberName, value) \
    !!!writeStructMember runningApp writeGlobalFrame bytecodeAppCache_t memberName value

void initializeBytecodeAppGlobalFrame(allocPointer_t runningApp) {
    allocPointer_t fileHandle = getRunningAppFileHandle(runningApp);
    int32_t functionTableLength = getBytecodeAppHeaderMember(fileHandle, functionTableLength);
    int32_t appDataPos = getBytecodeAppHeaderMember(fileHandle, appDataPos);
    setBytecodeAppCacheMember(runningApp, functionTableLength, functionTableLength);
    setBytecodeAppCacheMember(runningApp, appDataPos, appDataPos);
    printf("Function table length: %d\n", functionTableLength);
    printf("App data pos: %d\n", appDataPos);
}


