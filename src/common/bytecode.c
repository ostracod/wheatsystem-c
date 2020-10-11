
#include "./headers.h"

#define getArgPrefixReferenceType(argPrefix) (argPrefix >> 4)
#define getArgPrefixDataType(argPrefix) (argPrefix & 0x0F)

instructionArg_t instructionArgArray[MAXIMUM_ARG_AMOUNT];
allocPointer_t currentLocalFrame;
allocPointer_t currentImplementer;
allocPointer_t currentImplementerFileHandle;
int32_t currentInstructionFilePos;

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

int32_t readArgIntHelper(instructionArg_t *arg, int32_t offset, int8_t dataType) {
    uint8_t tempPrefix = arg->prefix;
    uint8_t referenceType = getArgPrefixReferenceType(tempPrefix);
    if (dataType < 0) {
        dataType = getArgPrefixDataType(tempPrefix);
    }
    if (referenceType == CONSTANT_REF_TYPE) {
        return arg->constantValue;
    } else if (referenceType == APP_DATA_REF_TYPE) {
        int32_t tempFilePos = getBytecodeGlobalFrameMember(
            currentImplementer,
            appDataFilePos
        ) + arg->appDataIndex + offset;
        if (dataType == SIGNED_INT_8_TYPE) {
            return readFile(currentImplementerFileHandle, tempFilePos, int8_t);
        } else {
            return readFile(currentImplementerFileHandle, tempFilePos, int32_t);
        }
    } else {
        heapMemoryOffset_t tempAddress = arg->address + offset;
        if (dataType == SIGNED_INT_8_TYPE) {
            return readHeapMemory(tempAddress, int8_t);
        } else {
            return readHeapMemory(tempAddress, int32_t);
        }
    }
}

void writeArgIntHelper(
    instructionArg_t *arg,
    int32_t offset,
    int8_t dataType,
    int32_t value
) {
    uint8_t tempPrefix = arg->prefix;
    uint8_t referenceType = getArgPrefixReferenceType(tempPrefix);
    if (dataType < 0) {
        dataType = getArgPrefixDataType(tempPrefix);
    }
    if (referenceType == HEAP_MEM_REF_TYPE) {
        heapMemoryOffset_t tempAddress = arg->address + offset;
        if (dataType == SIGNED_INT_8_TYPE) {
            writeHeapMemory(tempAddress, int8_t, (int8_t)value);
        } else {
            writeHeapMemory(tempAddress, int32_t, value);
        }
    } else {
        // TODO: Throw an error.
        
    }
}

instructionArg_t parseInstructionArg() {
    uint8_t argPrefix = readFileAndAdvance(
        currentImplementerFileHandle,
        currentInstructionFilePos,
        uint8_t
    );
    uint8_t referenceType = getArgPrefixReferenceType(argPrefix);
    uint8_t dataType = getArgPrefixDataType(argPrefix);
    instructionArg_t output;
    if (referenceType == CONSTANT_REF_TYPE) {
        output.prefix = argPrefix;
        if (dataType == SIGNED_INT_8_TYPE) {
            output.constantValue = readFileAndAdvance(
                currentImplementerFileHandle,
                currentInstructionFilePos,
                int8_t
            );
        } else {
            output.constantValue = readFileAndAdvance(
                currentImplementerFileHandle,
                currentInstructionFilePos,
                int32_t
            );
        }
    } else {
        instructionArg_t tempArg1 = parseInstructionArg();
        int32_t argValue1 = readArgIntHelper(&tempArg1, 0, -1);
        if (referenceType == APP_DATA_REF_TYPE) {
            output.prefix = argPrefix;
            output.appDataIndex = argValue1;
        } else {
            heapMemoryOffset_t baseAddress;
            heapMemoryOffset_t tempOffset;
            if (referenceType == DYNAMIC_ALLOC_REF_TYPE) {
                allocPointer_t tempPointer = (allocPointer_t)argValue1;
                baseAddress = getDynamicAllocDataAddress(tempPointer);
                instructionArg_t tempArg2 = parseInstructionArg();
                tempOffset = (heapMemoryOffset_t)readArgIntHelper(&tempArg2, 0, -1);
            } else {
                tempOffset = (heapMemoryOffset_t)argValue1;
                if (referenceType == GLOBAL_FRAME_REF_TYPE) {
                    baseAddress = getBytecodeGlobalFrameDataAddress(currentImplementer);
                } else if (referenceType == LOCAL_FRAME_REF_TYPE) {
                    baseAddress = getBytecodeLocalFrameDataAddress(currentLocalFrame);
                } else {
                    allocPointer_t tempLocalFrame;
                    if (referenceType == PREV_ARG_FRAME_REF_TYPE) {
                        tempLocalFrame = getLocalFrameMember(
                            currentLocalFrame,
                            previousLocalFrame
                        );
                    } else {
                        tempLocalFrame = currentLocalFrame;
                    }
                    allocPointer_t argFrame = getLocalFrameMember(tempLocalFrame, nextArgFrame);
                    baseAddress = getAllocDataAddress(argFrame);
                }
            }
            output.prefix = (HEAP_MEM_REF_TYPE << 4) | dataType;
            output.address = baseAddress + (heapMemoryOffset_t)tempOffset;
            // TODO: Set output.maximumAddress.
        }
    }
    return output;
}

void jumpToBytecodeInstruction(allocPointer_t localFrame, int32_t instructionOffset) {
    int32_t instructionBodyFilePos = getBytecodeLocalFrameMember(
        localFrame,
        instructionBodyStartFilePos
    );
    // TODO: Verify that instructionOffset is within bounds
    // of instruction body.
    setBytecodeLocalFrameMember(
        localFrame,
        instructionFilePos,
        instructionBodyFilePos + instructionOffset
    );
}


