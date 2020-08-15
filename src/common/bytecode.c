
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

int32_t findBytecodeFunction(allocPointer_t fileHandle, int32_t functionId) {
    int32_t functionTableLength = getBytecodeAppHeaderMember(fileHandle, functionTableLength);
    for (int32_t index = 0; index < functionTableLength; index++) {
        int32_t tempFunctionId = getBytecodeFunctionMember(fileHandle, index, functionId);
        if (tempFunctionId == functionId) {
            return index;
        }
    }
    return -1;
}


