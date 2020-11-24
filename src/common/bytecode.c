
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
typedef struct instructionArg {
    uint8_t prefix;
    union {
        // For HEAP_MEM_REF_TYPE, the union contains address and maximumAddress.
        // For CONSTANT_REF_TYPE, the union contains constantValue.
        // For APP_DATA_REF_TYPE, the union contains appDataIndex.
        struct {
            heapMemoryOffset_t address;
            heapMemoryOffset_t maximumAddress;
        };
        int32_t constantValue;
        int32_t appDataIndex;
    };
} instructionArg_t;
#pragma pack(pop)

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

instructionArg_t instructionArgArray[MAXIMUM_ARG_AMOUNT];
int32_t currentInstructionFilePos;

int32_t readArgIntHelper(instructionArg_t *arg, int32_t offset, int8_t dataType) {
    uint8_t tempPrefix = arg->prefix;
    uint8_t referenceType = getArgPrefixReferenceType(tempPrefix);
    if (dataType < 0) {
        dataType = getArgPrefixDataType(tempPrefix);
    }
    if (referenceType == CONSTANT_REF_TYPE) {
        return arg->constantValue;
    } else if (referenceType == APP_DATA_REF_TYPE) {
        int32_t index = arg->appDataIndex + offset;
        int8_t dataTypeSize = getArgDataTypeSize(dataType);
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

void parseInstructionArg(instructionArg_t *destination) {
    uint8_t argPrefix = readFileAndAdvance(
        currentImplementerFileHandle,
        currentInstructionFilePos,
        uint8_t
    );
    uint8_t referenceType = getArgPrefixReferenceType(argPrefix);
    uint8_t dataType = getArgPrefixDataType(argPrefix);
    if (dataType > SIGNED_INT_32_TYPE) {
        unhandledErrorCode = TYPE_ERR_CODE;
        return;
    }
    if (referenceType == CONSTANT_REF_TYPE) {
        destination->prefix = argPrefix;
        if (dataType == SIGNED_INT_8_TYPE) {
            destination->constantValue = readFileAndAdvance(
                currentImplementerFileHandle,
                currentInstructionFilePos,
                int8_t
            );
        } else {
            destination->constantValue = readFileAndAdvance(
                currentImplementerFileHandle,
                currentInstructionFilePos,
                int32_t
            );
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
            heapMemoryOffset_t baseAddress;
            heapMemoryOffset_t tempOffset;
            if (referenceType == DYNAMIC_ALLOC_REF_TYPE) {
                allocPointer_t tempPointer = (allocPointer_t)argValue1;
                validateAllocPointer(tempPointer);
                if (unhandledErrorCode != 0) {
                    return;
                }
                baseAddress = getDynamicAllocDataAddress(tempPointer);
                instructionArg_t tempArg2;
                parseInstructionArg(&tempArg2);
                if (unhandledErrorCode != 0) {
                    return;
                }
                tempOffset = (heapMemoryOffset_t)readArgIntHelper(&tempArg2, 0, -1);
                if (unhandledErrorCode != 0) {
                    return;
                }
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
                        if (tempLocalFrame == NULL_ALLOC_POINTER) {
                            unhandledErrorCode = ARG_FRAME_ERR_CODE;
                            return;
                        }
                    } else if (referenceType == NEXT_ARG_FRAME_REF_TYPE) {
                        tempLocalFrame = currentLocalFrame;
                    } else {
                        unhandledErrorCode = TYPE_ERR_CODE;
                        return;
                    }
                    allocPointer_t argFrame = getLocalFrameMember(tempLocalFrame, nextArgFrame);
                    if (argFrame == NULL_ALLOC_POINTER) {
                        unhandledErrorCode = ARG_FRAME_ERR_CODE;
                        return;
                    }
                    baseAddress = getAllocDataAddress(argFrame);
                }
            }
            destination->prefix = (HEAP_MEM_REF_TYPE << 4) | dataType;
            destination->address = baseAddress + (heapMemoryOffset_t)tempOffset;
            // TODO: Set output.maximumAddress.
            
        }
    }
}

void jumpToBytecodeInstruction(int32_t instructionOffset) {
    int32_t instructionBodyFilePos = getBytecodeLocalFrameMember(
        currentLocalFrame,
        instructionBodyStartFilePos
    );
    // TODO: Verify that instructionOffset is within bounds
    // of instruction body.
    setBytecodeLocalFrameMember(
        currentLocalFrame,
        instructionFilePos,
        instructionBodyFilePos + instructionOffset
    );
}

void evaluateBytecodeInstruction() {
    int32_t instructionBodyEndFilePos = getBytecodeLocalFrameMember(
        currentLocalFrame,
        instructionBodyEndFilePos
    );
    currentInstructionFilePos = getBytecodeLocalFrameMember(
        currentLocalFrame,
        instructionFilePos
    );
    if (currentInstructionFilePos >= instructionBodyEndFilePos) {
        returnFromFunction();
        return;
    }
    uint8_t opcode = readFileAndAdvance(
        currentImplementerFileHandle,
        currentInstructionFilePos,
        uint8_t
    );
    printDebugNumber(opcode);
    printDebugNewline();
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
            cleanUpNextArgFrame();
            heapMemoryOffset_t argFrameSize = (heapMemoryOffset_t)readArgInt(0);
            allocPointer_t nextArgFrame = createAlloc(ARG_FRAME_ALLOC_TYPE, argFrameSize);
            setLocalFrameMember(currentLocalFrame, nextArgFrame, nextArgFrame);
        } else if (opcodeOffset == 0x3) {
            // newAlloc.
            int8_t isGuarded = (int8_t)readArgInt(1);
            heapMemoryOffset_t tempSize = (heapMemoryOffset_t)readArgInt(2);
            allocPointer_t tempAlloc = createDynamicAlloc(
                tempSize,
                isGuarded,
                currentImplementerFileHandle
            );
            writeArgInt(0, tempAlloc);
        } else if (opcodeOffset == 0x4) {
            // delAlloc.
            allocPointer_t tempAlloc = (allocPointer_t)readArgInt(0);
            // TODO: Throw error if pointer is invalid.
            deleteAlloc(tempAlloc);
        } else if (opcodeOffset == 0x5) {
            // allocSize.
            allocPointer_t tempAlloc = (allocPointer_t)readArgInt(1);
            heapMemoryOffset_t tempSize = getDynamicAllocSize(tempAlloc);
            writeArgInt(0, tempSize);
        } else if (opcodeOffset == 0x6) {
            // allocCreator.
            allocPointer_t tempAlloc = (allocPointer_t)readArgInt(1);
            allocPointer_t tempCreator = getDynamicAllocMember(tempAlloc, creator);
            writeArgInt(0, tempCreator);
        }
    } else if (opcodeCategory == 0x1) {
        // Control flow instructions.
        if (opcodeOffset == 0x0) {
            // jmp.
            int32_t instructionOffset = readArgInt(0);
            jumpToBytecodeInstruction(instructionOffset);
        } else if (opcodeOffset == 0x1) {
            // jmpZ.
            int32_t condition = readArgInt(1);
            if (condition == 0) {
                int32_t instructionOffset = readArgInt(0);
                jumpToBytecodeInstruction(instructionOffset);
            }
        } else if (opcodeOffset == 0x2) {
            // jmpNZ.
            int32_t condition = readArgInt(1);
            if (condition != 0) {
                int32_t instructionOffset = readArgInt(0);
                jumpToBytecodeInstruction(instructionOffset);
            }
        }
    } else if (opcodeCategory == 0x2) {
        // Error instructions.
        if (opcodeOffset == 0x0) {
            // setErrJmp.
            int32_t instructionOffset = readArgInt(0);
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
        }
    } else if (opcodeCategory == 0x3) {
        // Function instructions.
        if (opcodeOffset == 0x0) {
            // findFunc.
            allocPointer_t appHandle = (allocPointer_t)readArgInt(1);
            allocPointer_t tempRunningApp = getFileHandleRunningApp(appHandle);
            int32_t functionId = readArgInt(2);
            int32_t functionIndex = findFunctionById(tempRunningApp, functionId);
            writeArgInt(0, functionIndex);
        } else if (opcodeOffset == 0x1) {
            // call.
            int32_t functionIndex = readArgInt(0);
            callFunction(
                currentThreadApp,
                currentImplementer,
                currentImplementer,
                functionIndex
            );
        } else if (opcodeOffset == 0x2) {
            // callRemote.
            allocPointer_t appHandle = (allocPointer_t)readArgInt(0);
            int32_t functionIndex = readArgInt(1);
            allocPointer_t tempImplementer = getFileHandleRunningApp(appHandle);
            callFunction(
                currentThreadApp,
                currentImplementer,
                tempImplementer,
                functionIndex
            );
        } else if (opcodeOffset == 0x3) {
            // ret.
            returnFromFunction();
        } else if (opcodeOffset == 0x4) {
            // caller.
            allocPointer_t tempCaller = getLocalFrameMember(currentLocalFrame, caller);
            writeArgInt(0, tempCaller);
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
        }
        writeArgInt(0, result);
    } else if (opcodeCategory == 0x7) {
        // Application instructions.
        if (opcodeOffset == 0x0) {
            // launch.
            allocPointer_t appHandle = readArgInt(0);
            launchApp(appHandle);
        }
    } else if (opcodeCategory == 0x8) {
        // File instructions.
        if (opcodeOffset == 0x2) {
            // openFile.
            allocPointer_t fileName = readArgInt(1);
            allocPointer_t fileHandle = openFileByStringAlloc(fileName);
            writeArgInt(0, fileHandle);
        } else if (opcodeOffset == 0x3) {
            // closeFile.
            allocPointer_t fileHandle = readArgInt(0);
            closeFile(fileHandle);
        }
    }
}


