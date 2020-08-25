
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

int32_t getArgValueHelper(instructionArg_t *arg) {
    uint8_t tempPrefix = arg->prefix;
    uint8_t referenceType = getArgPrefixReferenceType(tempPrefix);
    uint8_t dataType = getArgPrefixDataType(tempPrefix);
    if (referenceType == CONSTANT_REF_TYPE) {
        return arg->constantValue;
    } else if (referenceType == APP_DATA_REF_TYPE) {
        int32_t tempFilePos = getBytecodeGlobalFrameMember(
            currentImplementer,
            appDataFilePos
        ) + arg->appDataIndex;
        if (dataType == SIGNED_INT_8_TYPE) {
            return readFile(currentImplementerFileHandle, tempFilePos, int8_t);
        } else {
            return readFile(currentImplementerFileHandle, tempFilePos, int32_t);
        }
    } else {
        if (dataType == SIGNED_INT_8_TYPE) {
            return readHeapMemory(arg->address, int8_t);
        } else {
            return readHeapMemory(arg->address, int32_t);
        }
    }
}

void setArgValue(int8_t index, int32_t value) {
    instructionArg_t *arg = instructionArgArray + index;
    uint8_t tempPrefix = arg->prefix;
    uint8_t referenceType = getArgPrefixReferenceType(tempPrefix);
    uint8_t dataType = getArgPrefixDataType(tempPrefix);
    if (referenceType == HEAP_MEM_REF_TYPE) {
        if (dataType == SIGNED_INT_8_TYPE) {
            writeHeapMemory(arg->address, int8_t, (int8_t)value);
        } else {
            writeHeapMemory(arg->address, int32_t, value);
        }
        printf("WROTE VALUE %d AT ADDRESS %d\n", value, arg->address);
    } else {
        // TODO: Throw an error.
        
    }
}

instructionArg_t readInstructionArg() {
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
    } else if (referenceType == DYNAMIC_ALLOC_REF_TYPE) {
        // TODO: Implement.
        
    } else {
        instructionArg_t tempArg = readInstructionArg();
        int32_t tempOffset = getArgValueHelper(&tempArg);
        if (referenceType == APP_DATA_REF_TYPE) {
            output.prefix = argPrefix;
            output.appDataIndex = tempOffset;
        } else {
            heapMemoryOffset_t baseAddress;
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
            output.prefix = (HEAP_MEM_REF_TYPE << 4) | dataType;
            output.address = baseAddress + (heapMemoryOffset_t)tempOffset;
            // TODO: Set output.maximumAddress.
        }
    }
    return output;
}


