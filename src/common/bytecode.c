
#include "./headers.h"

#define CONSTANT_REF_TYPE 0x0
#define GLOBAL_FRAME_REF_TYPE 0x1
#define LOCAL_FRAME_REF_TYPE 0x2
#define PREV_ARG_FRAME_REF_TYPE 0x3
#define NEXT_ARG_FRAME_REF_TYPE 0x4
#define APP_DATA_REF_TYPE 0x5
#define DYNAMIC_ALLOC_REF_TYPE 0x6
#define HEAP_MEM_REF_TYPE 0x7

#define SIGNED_INT_8_TYPE 0x0
#define SIGNED_INT_32_TYPE 0x1

#pragma pack(push, 1)
typedef struct instructionArg_t {
    uint8_t prefix;
    union {
        // For HEAP_MEM_REF_TYPE, the union contains startAddress, size, and index.
        // For CONSTANT_REF_TYPE, the union contains constantValue.
        // For APP_DATA_REF_TYPE, the union contains appDataIndex.
        struct {
            heapMemoryOffset_t startAddress;
            heapMemoryOffset_t index;
            heapMemoryOffset_t size;
        };
        int32_t constantValue;
        int32_t appDataIndex;
    };
} instructionArg_t;
#pragma pack(pop)

#define readInstructionData(type) ({ \
    if (currentInstructionFilePos < instructionBodyStartFilePos || currentInstructionFilePos + sizeof(type) > instructionBodyEndFilePos) { \
        unhandledErrorCode = INDEX_ERR_CODE; \
        return; \
    } \
    type result = readFile(currentImplementerFileHandle, currentInstructionFilePos, type); \
    currentInstructionFilePos += sizeof(type); \
    result; \
})

#define getArgPrefixReferenceType(argPrefix) (argPrefix >> 4)
#define getArgPrefixDataType(argPrefix) (argPrefix & 0x0F)

#define getArgDataTypeSize(dataType) (dataType == SIGNED_INT_8_TYPE ? 1 : 4)

#define readArgInt(index) ({ \
    int32_t tempResult = readArgIntHelper(instructionArgArray + index, 0, -1); \
    if (unhandledErrorCode != 0) { \
        return; \
    } \
    tempResult; \
})
#define writeArgInt(index, value) \
    writeArgIntHelper(instructionArgArray + index, 0, -1, value); \
    if (unhandledErrorCode != 0) { \
        return; \
    }

#define readArgConstantInt(index) ({ \
    int32_t tempResult = readArgConstantIntHelper(index); \
    if (unhandledErrorCode != 0) { \
        return; \
    } \
    tempResult; \
});
#define readArgDynamicAlloc(index) ({ \
    allocPointer_t(dynamicAlloc_t) pointer = castGenericPointer(readArgInt(index), dynamicAlloc_t); \
    validateDynamicAlloc(pointer); \
    pointer; \
})

#define readArgFileHandle(index) ({ \
    allocPointer_t(fileHandle_t) fileHandle = castGenericPointer(readArgInt(index), fileHandle_t); \
    validateFileHandle(fileHandle); \
    fileHandle; \
});
#define readArgRunningApp(index) ({ \
    allocPointer_t(runningApp_t) runningApp; \
    readArgRunningAppHelper(&runningApp, index); \
    runningApp; \
});

instructionArg_t instructionArgArray[MAXIMUM_ARG_AMOUNT];
int32_t currentInstructionFilePos;
int32_t instructionBodyStartFilePos;
int32_t instructionBodyEndFilePos;

heapMemoryOffset_t getArgHeapMemoryAddress(
    instructionArg_t *arg,
    int32_t offset,
    int8_t dataTypeSize
) {
    heapMemoryOffset_t index = arg->index + offset;
    if (index < 0 || index + dataTypeSize > arg->size) {
        unhandledErrorCode = INDEX_ERR_CODE;
    }
    return arg->startAddress + index;
}

int32_t getArgBufferSize(instructionArg_t *arg) {
    uint8_t referenceType = getArgPrefixReferenceType(arg->prefix);
    if (referenceType == CONSTANT_REF_TYPE) {
        return -1;
    } else if (referenceType == APP_DATA_REF_TYPE) {
        int32_t appDataSize = getBytecodeGlobalFrameMember(currentImplementer, appDataSize);
        return appDataSize - arg->appDataIndex;
    } else {
        return arg->size - arg->index;
    }
}

int32_t readArgIntHelper(instructionArg_t *arg, int32_t offset, int8_t dataType) {
    uint8_t tempPrefix = arg->prefix;
    uint8_t referenceType = getArgPrefixReferenceType(tempPrefix);
    if (dataType < 0) {
        dataType = getArgPrefixDataType(tempPrefix);
    }
    int8_t dataTypeSize = getArgDataTypeSize(dataType);
    if (referenceType == CONSTANT_REF_TYPE) {
        return arg->constantValue;
    } else if (referenceType == APP_DATA_REF_TYPE) {
        int32_t index = arg->appDataIndex + offset;
        int32_t appDataSize = getBytecodeGlobalFrameMember(currentImplementer, appDataSize);
        if (index < 0 || index + dataTypeSize > appDataSize) {
            unhandledErrorCode = INDEX_ERR_CODE;
            return 0;
        }
        int32_t tempFilePos = getBytecodeGlobalFrameMember(
            currentImplementer,
            appDataFilePos
        ) + index;
        if (dataType == SIGNED_INT_8_TYPE) {
            return readFile(currentImplementerFileHandle, tempFilePos, int8_t);
        } else {
            return readFile(currentImplementerFileHandle, tempFilePos, int32_t);
        }
    } else {
        heapMemoryOffset_t tempAddress = getArgHeapMemoryAddress(arg, offset, dataTypeSize);
        if (unhandledErrorCode != 0) {
            return 0;
        }
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
    int8_t dataTypeSize = getArgDataTypeSize(dataType);
    if (referenceType == HEAP_MEM_REF_TYPE) {
        heapMemoryOffset_t tempAddress = getArgHeapMemoryAddress(arg, offset, dataTypeSize);
        if (unhandledErrorCode != 0) {
            return;
        }
        if (dataType == SIGNED_INT_8_TYPE) {
            writeHeapMemory(tempAddress, int8_t, (int8_t)value);
        } else {
            writeHeapMemory(tempAddress, int32_t, value);
        }
    } else {
        unhandledErrorCode = TYPE_ERR_CODE;
    }
}

void parseInstructionArg(instructionArg_t *destination) {
    uint8_t argPrefix = readInstructionData(uint8_t);
    uint8_t referenceType = getArgPrefixReferenceType(argPrefix);
    uint8_t dataType = getArgPrefixDataType(argPrefix);
    if (dataType > SIGNED_INT_32_TYPE) {
        unhandledErrorCode = TYPE_ERR_CODE;
        return;
    }
    if (referenceType == CONSTANT_REF_TYPE) {
        destination->prefix = argPrefix;
        if (dataType == SIGNED_INT_8_TYPE) {
            destination->constantValue = readInstructionData(int8_t);
        } else {
            destination->constantValue = readInstructionData(int32_t);
        }
    } else {
        instructionArg_t tempArg1;
        parseInstructionArg(&tempArg1);
        if (unhandledErrorCode != 0) {
            return;
        }
        int32_t argValue1 = readArgIntHelper(&tempArg1, 0, -1);
        if (unhandledErrorCode != 0) {
            return;
        }
        if (referenceType == APP_DATA_REF_TYPE) {
            destination->prefix = argPrefix;
            destination->appDataIndex = argValue1;
        } else {
            heapMemoryOffset_t startAddress;
            heapMemoryOffset_t tempSize;
            heapMemoryOffset_t index;
            if (referenceType == DYNAMIC_ALLOC_REF_TYPE) {
                allocPointer_t(dynamicAlloc_t) tempPointer = castGenericPointer(
                    argValue1,
                    dynamicAlloc_t
                );
                validateDynamicAlloc(tempPointer);
                if (unhandledErrorCode != 0) {
                    return;
                }
                startAddress = getDynamicAllocDataAddress(tempPointer);
                instructionArg_t tempArg2;
                parseInstructionArg(&tempArg2);
                if (unhandledErrorCode != 0) {
                    return;
                }
                index = (heapMemoryOffset_t)readArgIntHelper(&tempArg2, 0, -1);
                if (unhandledErrorCode != 0) {
                    return;
                }
                tempSize = getDynamicAllocSize(tempPointer);
            } else {
                index = (heapMemoryOffset_t)argValue1;
                if (referenceType == GLOBAL_FRAME_REF_TYPE) {
                    startAddress = getBytecodeGlobalFrameDataAddress(currentImplementer);
                    tempSize = getBytecodeGlobalFrameSize(currentImplementer);
                } else if (referenceType == LOCAL_FRAME_REF_TYPE) {
                    startAddress = getBytecodeLocalFrameDataAddress(currentLocalFrame);
                    tempSize = getBytecodeLocalFrameSize(currentLocalFrame);
                } else {
                    allocPointer_t(localFrame_t) tempLocalFrame;
                    if (referenceType == PREV_ARG_FRAME_REF_TYPE) {
                        tempLocalFrame = getLocalFrameMember(
                            currentLocalFrame,
                            previousLocalFrame
                        );
                        if (pointerIsNull(tempLocalFrame)) {
                            unhandledErrorCode = ARG_FRAME_ERR_CODE;
                            return;
                        }
                    } else if (referenceType == NEXT_ARG_FRAME_REF_TYPE) {
                        tempLocalFrame = currentLocalFrame;
                    } else {
                        unhandledErrorCode = TYPE_ERR_CODE;
                        return;
                    }
                    allocPointer_t(argFrame_t) argFrame = getLocalFrameMember(
                        tempLocalFrame,
                        nextArgFrame
                    );
                    if (pointerIsNull(argFrame)) {
                        unhandledErrorCode = ARG_FRAME_ERR_CODE;
                        return;
                    }
                    startAddress = getArgFrameDataAddress(argFrame);
                    tempSize = getArgFrameSize(argFrame);
                }
            }
            destination->prefix = (HEAP_MEM_REF_TYPE << 4) | dataType;
            destination->startAddress = startAddress;
            destination->size = tempSize;
            destination->index = index;
        }
    }
}

void jumpToBytecodeInstruction(int32_t instructionOffset) {
    int32_t instructionBodyFilePos = getBytecodeLocalFrameMember(
        currentLocalFrame,
        instructionBodyStartFilePos
    );
    setBytecodeLocalFrameMember(
        currentLocalFrame,
        instructionFilePos,
        instructionBodyFilePos + instructionOffset
    );
}

int32_t readArgConstantIntHelper(int8_t index) {
    instructionArg_t *tempArg = instructionArgArray + index;
    uint8_t referenceType = getArgPrefixReferenceType(tempArg->prefix);
    if (referenceType != CONSTANT_REF_TYPE) {
        unhandledErrorCode = TYPE_ERR_CODE;
    }
    return tempArg->constantValue;
}

void readArgRunningAppHelper(allocPointer_t(runningApp_t) *destination, int8_t index) {
    allocPointer_t(fileHandle_t) appHandle = readArgFileHandle(index);
    allocPointer_t(runningApp_t) runningApp = getFileHandleRunningApp(appHandle);
    if (pointerIsNull(runningApp)) {
        if (getFileHandleType(runningApp) == GENERIC_FILE_TYPE) {
            unhandledErrorCode = TYPE_ERR_CODE;
        } else {
            unhandledErrorCode = STATE_ERR_CODE;
        }
        return;
    }
    *destination = runningApp;
}

void evaluateBytecodeInstruction() {
    instructionBodyStartFilePos = getBytecodeLocalFrameMember(
        currentLocalFrame,
        instructionBodyStartFilePos
    );
    instructionBodyEndFilePos = getBytecodeLocalFrameMember(
        currentLocalFrame,
        instructionBodyEndFilePos
    );
    currentInstructionFilePos = getBytecodeLocalFrameMember(
        currentLocalFrame,
        instructionFilePos
    );
    if (currentInstructionFilePos == instructionBodyEndFilePos) {
        returnFromFunction();
        return;
    }
    uint8_t opcode = readInstructionData(uint8_t);
    //printDebugString((int8_t *)"OPCODE ");
    //printDebugNumber(opcode);
    //printDebugNewline();
    uint8_t opcodeCategory = opcode >> 4;
    uint8_t opcodeOffset = opcode & 0x0F;
    int8_t tempOffset = readArrayConstantElement(
        argumentAmountOffsetArray,
        opcodeCategory
    );
    int8_t argumentAmount = readArrayConstantElement(
        argumentAmountArray,
        tempOffset + opcodeOffset
    );
    for (int8_t index = 0; index < argumentAmount; index++) {
        parseInstructionArg(instructionArgArray + index);
        if (unhandledErrorCode != 0) {
            return;
        }
    }
    setBytecodeLocalFrameMember(
        currentLocalFrame,
        instructionFilePos,
        currentInstructionFilePos
    );
    if (opcodeCategory == 0x0) {
        // Memory instructions.
        if (opcodeOffset == 0x0) {
            // wrt.
            int32_t tempValue = readArgInt(1);
            writeArgInt(0, tempValue);
        } else if (opcodeOffset == 0x1) {
            // wrtBuff.
            instructionArg_t *destination = instructionArgArray;
            instructionArg_t *source = instructionArgArray + 1;
            int32_t size = readArgInt(2);
            int32_t destinationBufferSize = getArgBufferSize(destination);
            int32_t sourceBufferSize = getArgBufferSize(source);
            if (size < 0 || size > destinationBufferSize || size > sourceBufferSize) {
                unhandledErrorCode = NUM_RANGE_ERR_CODE;
                return;
            }
            for (int32_t offset = 0; offset < size; offset++) {
                int8_t tempValue = readArgIntHelper(source, offset, SIGNED_INT_8_TYPE);
                if (unhandledErrorCode != 0) {
                    return;
                }
                writeArgIntHelper(destination, offset, SIGNED_INT_8_TYPE, tempValue);
                if (unhandledErrorCode != 0) {
                    return;
                }
            }
        } else if (opcodeOffset == 0x2) {
            // newArgFrame.
            heapMemoryOffset_t argFrameSize = (heapMemoryOffset_t)readArgInt(0);
            if (argFrameSize < 0) {
                unhandledErrorCode = NUM_RANGE_ERR_CODE;
                return;
            }
            createNextArgFrame(argFrameSize);
        } else if (opcodeOffset == 0x3) {
            // newAlloc.
            int8_t isGuarded = (int8_t)readArgInt(1);
            heapMemoryOffset_t tempSize = (heapMemoryOffset_t)readArgInt(2);
            if (tempSize < 0) {
                unhandledErrorCode = NUM_RANGE_ERR_CODE;
                return;
            }
            allocPointer_t(dynamicAlloc_t) tempAlloc = createDynamicAlloc(
                tempSize,
                isGuarded,
                currentImplementerFileHandle
            );
            writeArgInt(0, tempAlloc.genericPointer);
        } else if (opcodeOffset == 0x4) {
            // delAlloc.
            allocPointer_t(dynamicAlloc_t) tempAlloc = readArgDynamicAlloc(0);
            deleteAlloc(tempAlloc.genericPointer);
        } else if (opcodeOffset == 0x5) {
            // allocSize.
            allocPointer_t(dynamicAlloc_t) tempAlloc = readArgDynamicAlloc(1);
            heapMemoryOffset_t tempSize = getDynamicAllocSize(tempAlloc);
            writeArgInt(0, tempSize);
        } else if (opcodeOffset == 0x6) {
            // allocCreator.
            allocPointer_t(dynamicAlloc_t) tempAlloc = readArgDynamicAlloc(1);
            allocPointer_t(fileHandle_t) tempCreator = getDynamicAllocMember(
                tempAlloc,
                creator
            );
            writeArgInt(0, tempCreator.genericPointer);
        } else {
            unhandledErrorCode = NO_IMPL_ERR_CODE;
            return;
        }
    } else if (opcodeCategory == 0x1) {
        // Control flow instructions.
        if (opcodeOffset == 0x0) {
            // jmp.
            int32_t instructionOffset = readArgConstantInt(0);
            jumpToBytecodeInstruction(instructionOffset);
        } else if (opcodeOffset == 0x1) {
            // jmpZ.
            int32_t condition = readArgInt(1);
            if (condition == 0) {
                int32_t instructionOffset = readArgConstantInt(0);
                jumpToBytecodeInstruction(instructionOffset);
            }
        } else if (opcodeOffset == 0x2) {
            // jmpNZ.
            int32_t condition = readArgInt(1);
            if (condition != 0) {
                int32_t instructionOffset = readArgConstantInt(0);
                jumpToBytecodeInstruction(instructionOffset);
            }
        } else {
            unhandledErrorCode = NO_IMPL_ERR_CODE;
            return;
        }
    } else if (opcodeCategory == 0x2) {
        // Error instructions.
        if (opcodeOffset == 0x0) {
            // setErrJmp.
            int32_t instructionOffset = readArgConstantInt(0);
            setBytecodeLocalFrameMember(currentLocalFrame, errorHandler, instructionOffset);
        } else if (opcodeOffset == 0x1) {
            // clrErrJmp.
            setBytecodeLocalFrameMember(currentLocalFrame, errorHandler, -1);
        } else if (opcodeOffset == 0x2) {
            // throw.
            int32_t tempCode = readArgInt(0);
            if (tempCode < -128 || tempCode > 127) {
                unhandledErrorCode = NUM_RANGE_ERR_CODE;
                return;
            }
            unhandledErrorCode = tempCode;
            return;
        } else if (opcodeOffset == 0x3) {
            // err.
            int8_t tempCode = getLocalFrameMember(currentLocalFrame, lastErrorCode);
            writeArgInt(0, tempCode);
        } else {
            unhandledErrorCode = NO_IMPL_ERR_CODE;
            return;
        }
    } else if (opcodeCategory == 0x3) {
        // Function instructions.
        if (opcodeOffset == 0x0) {
            // findFunc.
            allocPointer_t(runningApp_t) runningApp = readArgRunningApp(1);
            int32_t functionId = readArgInt(2);
            int32_t functionIndex = findFunctionById(runningApp, functionId);
            writeArgInt(0, functionIndex);
        } else if (opcodeOffset == 0x1) {
            // call.
            int32_t functionIndex = readArgInt(0);
            callFunction(
                currentThreadApp,
                currentImplementer,
                functionIndex
            );
        } else if (opcodeOffset == 0x2) {
            // callRemote.
            allocPointer_t(runningApp_t) tempImplementer = readArgRunningApp(0);
            int32_t functionIndex = readArgInt(1);
            callFunction(
                currentThreadApp,
                tempImplementer,
                functionIndex
            );
        } else if (opcodeOffset == 0x3) {
            // ret.
            returnFromFunction();
        } else if (opcodeOffset == 0x4) {
            // caller.
            allocPointer_t(runningApp_t) tempCaller = getCurrentCaller();
            allocPointer_t(fileHandle_t) fileHandle;
            if (pointerIsNull(tempCaller)) {
                fileHandle = nullAllocPointer(fileHandle_t);
            } else {
                fileHandle = getRunningAppMember(tempCaller, fileHandle);
            }
            writeArgInt(0, fileHandle.genericPointer);
        } else {
            unhandledErrorCode = NO_IMPL_ERR_CODE;
            return;
        }
    } else if (opcodeCategory == 0x4) {
        // Bitwise instructions.
        uint32_t result = 0;
        if (opcodeOffset == 0x0) {
            // bNot.
            uint32_t operand = readArgInt(1);
            result = ~operand;
        } else {
            uint32_t operand1 = readArgInt(1);
            uint32_t operand2 = readArgInt(2);
            if (opcodeOffset == 0x1) {
                // bOr.
                result = (operand1 | operand2);
            } else if (opcodeOffset == 0x2) {
                // bAnd.
                result = (operand1 & operand2);
            } else if (opcodeOffset == 0x3) {
                // bXor.
                result = (operand1 ^ operand2);
            } else if (opcodeOffset == 0x4) {
                // bLeft.
                result = (operand1 << operand2);
            } else if (opcodeOffset == 0x5) {
                // bRight.
                result = (operand1 >> operand2);
            } else {
                unhandledErrorCode = NO_IMPL_ERR_CODE;
                return;
            }
        }
        writeArgInt(0, result);
    } else if (opcodeCategory == 0x5) {
        // Comparison instructions.
        int32_t operand1 = readArgInt(1);
        int32_t operand2 = readArgInt(2);
        int32_t result = 0;
        if (opcodeOffset == 0x0) {
            // equ.
            result = (operand1 == operand2);
        } else if (opcodeOffset == 0x1) {
            // nEqu.
            result = (operand1 != operand2);
        } else if (opcodeOffset == 0x2) {
            // gre.
            result = (operand1 > operand2);
        } else if (opcodeOffset == 0x3) {
            // nGre.
            result = (operand1 <= operand2);
        } else {
            unhandledErrorCode = NO_IMPL_ERR_CODE;
            return;
        }
        writeArgInt(0, result);
    } else if (opcodeCategory == 0x6) {
        // Arithmetic instructions.
        int32_t operand1 = readArgInt(1);
        int32_t operand2 = readArgInt(2);
        int32_t result = 0;
        if (opcodeOffset == 0x0) {
            // add.
            result = operand1 + operand2;
        } else if (opcodeOffset == 0x1) {
            // sub.
            result = operand1 - operand2;
        } else if (opcodeOffset == 0x2) {
            // mul.
            result = operand1 * operand2;
        } else if (opcodeOffset == 0x3) {
            // div.
            if (operand2 == 0) {
                unhandledErrorCode = NUM_RANGE_ERR_CODE;
                return;
            }
            result = operand1 / operand2;
        } else if (opcodeOffset == 0x4) {
            // mod.
            if (operand2 == 0) {
                unhandledErrorCode = NUM_RANGE_ERR_CODE;
                return;
            }
            result = operand1 % operand2;
        } else {
            unhandledErrorCode = NO_IMPL_ERR_CODE;
            return;
        }
        writeArgInt(0, result);
    } else if (opcodeCategory == 0x7) {
        // Application instructions.
        if (opcodeOffset == 0x0) {
            // launch.
            allocPointer_t(fileHandle_t) appHandle = readArgFileHandle(0);
            launchApp(appHandle);
        } else {
            unhandledErrorCode = NO_IMPL_ERR_CODE;
            return;
        }
    } else if (opcodeCategory == 0x8) {
        // File instructions.
        if (opcodeOffset == 0x2) {
            // openFile.
            allocPointer_t(dynamicAlloc_t) fileName = readArgDynamicAlloc(1);
            allocPointer_t(fileHandle_t) fileHandle = openFileByDynamicStringAlloc(fileName);
            if (pointerIsNull(fileHandle)) {
                unhandledErrorCode = MISSING_ERR_CODE;
                return;
            }
            writeArgInt(0, fileHandle.genericPointer);
        } else if (opcodeOffset == 0x3) {
            // closeFile.
            allocPointer_t(fileHandle_t) fileHandle = readArgFileHandle(0);
            closeFile(fileHandle);
        } else {
            unhandledErrorCode = NO_IMPL_ERR_CODE;
            return;
        }
    } else {
        unhandledErrorCode = NO_IMPL_ERR_CODE;
        return;
    }
}


