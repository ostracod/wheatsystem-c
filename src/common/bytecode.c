
#include "./headers.h"

int32_t findBytecodeFunction(allocPointer_t fileHandle, int32_t functionId) {
    int32_t functionTableLength = getBytecodeAppMember(fileHandle, functionTableLength);
    for (int32_t index = 0; index < functionTableLength; index++) {
        int32_t tempFunctionId = getBytecodeFunctionMember(fileHandle, index, functionId);
        if (tempFunctionId == functionId) {
            return index;
        }
    }
    return -1;
}


