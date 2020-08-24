
#include "./headers.h"

instructionArg_t instructionArgArray[MAXIMUM_ARG_AMOUNT];

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

instructionArg_t readInstructionArg(int32_t *instructionFilePos, allocPointer_t localFrame) {
    // TODO: Implement.
    *instructionFilePos += 1;
    instructionArg_t output;
    
    return output;
}


