
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

int32_t readArg(instructionArg_t *arg) {
    uint8_t tempPrefix = arg->prefix;
    uint8_t referenceType = tempPrefix >> 4;
    uint8_t dataType = tempPrefix & 0x0F;
    if (referenceType == CONSTANT_REF_TYPE) {
        return arg->constantValue;
    } else if (referenceType == APP_DATA_REF_TYPE) {
        // TODO: Read app data.
        
    } else {
        // TODO: Read heap memory.
        
    }
}

instructionArg_t readInstructionArg(argParseContext_t *context) {
    uint8_t argPrefix = readFileAndAdvance(
        context->fileHandle,
        context->instructionFilePos,
        uint8_t
    );
    uint8_t referenceType = argPrefix >> 4;
    uint8_t dataType = argPrefix & 0x0F;
    instructionArg_t output;
    if (referenceType == CONSTANT_REF_TYPE) {
        output.prefix = argPrefix;
        if (dataType == SIGNED_INT_8_TYPE) {
            output.constantValue = readFileAndAdvance(
                context->fileHandle,
                context->instructionFilePos,
                int8_t
            );
        } else {
            output.constantValue = readFileAndAdvance(
                context->fileHandle,
                context->instructionFilePos,
                int32_t
            );
        }
        printf("CONSTANT ARG: %d %d\n", output.prefix, output.constantValue);
    } else if (referenceType == DYNAMIC_ALLOC_REF_TYPE) {
        // TODO: Implement.
        
    } else {
        instructionArg_t tempArg = readInstructionArg(context);
        int32_t tempOffset = readArg(&tempArg);
        if (referenceType == APP_DATA_REF_TYPE) {
            output.prefix = argPrefix;
            output.appDataIndex = tempOffset;
            printf("APP DATA ARG: %d %d\n", output.prefix, output.appDataIndex);
        } else {
            heapMemoryOffset_t baseAddress;
            if (referenceType == GLOBAL_FRAME_REF_TYPE) {
                baseAddress = getBytecodeGlobalFrameDataAddress(context->implementer);
            } else if (referenceType == LOCAL_FRAME_REF_TYPE) {
                baseAddress = getBytecodeLocalFrameDataAddress(context->localFrame);
            } else {
                allocPointer_t tempLocalFrame;
                if (referenceType == PREV_ARG_FRAME_REF_TYPE) {
                    tempLocalFrame = getLocalFrameMember(
                        context->localFrame,
                        previousLocalFrame
                    );
                } else {
                    tempLocalFrame = context->localFrame;
                }
                allocPointer_t argFrame = getLocalFrameMember(tempLocalFrame, nextArgFrame);
                baseAddress = getAllocDataAddress(argFrame);
            }
            output.prefix = (HEAP_MEM_REF_TYPE << 4) | dataType;
            output.address = baseAddress + (heapMemoryOffset_t)tempOffset;
            // TODO: Set output.maximumAddress.
            printf("HEAP MEM ARG: %d %d\n", output.prefix, output.address);
        }
    }
    return output;
}


