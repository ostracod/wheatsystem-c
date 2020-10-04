
#include "./headers.h"

declareArrayConstantWithValue(OPCODE_DEBUG_TEXT, "Opcode: ");

void launchApp(allocPointer_t fileHandle) {
    
    // Do not launch app again if it is already running.
    allocPointer_t runningApp = getFileHandleRunningApp(fileHandle);
    if (runningApp != NULL_ALLOC_POINTER) {
        return;
    }
    int8_t fileType = getFileHandleType(fileHandle);
    
    // Determine global frame size.
    heapMemoryOffset_t globalFrameSize;
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        globalFrameSize = sizeof(bytecodeGlobalFrameHeader_t) + (heapMemoryOffset_t)getBytecodeAppMember(fileHandle, globalFrameSize);
    } else if (fileType == SYSTEM_APP_FILE_TYPE) {
        // TODO: Determine size of global frame for system application.
        
        globalFrameSize = 0;
    } else {
        // TODO: Throw an error.
        return;
    }
    
    // Create allocation for the running app.
    runningApp = createAlloc(
        RUNNING_APP_ALLOC_TYPE,
        sizeof(runningAppHeader_t) + globalFrameSize
    );
    setRunningAppMember(runningApp, fileHandle, fileHandle);
    setRunningAppMember(runningApp, localFrame, NULL_ALLOC_POINTER);
    setRunningAppMember(runningApp, isWaiting, false);
    setFileHandleRunningApp(fileHandle, runningApp);
    
    // Initialize global frame.
    for (heapMemoryOffset_t index = 0; index < globalFrameSize; index++) {
        writeGlobalFrame(runningApp, index, int8_t, 0);
    }
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        int32_t functionTableLength = getBytecodeAppMember(fileHandle, functionTableLength);
        int32_t appDataFilePos = getBytecodeAppMember(fileHandle, appDataFilePos);
        setBytecodeGlobalFrameMember(runningApp, functionTableLength, functionTableLength);
        setBytecodeGlobalFrameMember(runningApp, appDataFilePos, appDataFilePos);
    }
    
    // Call init function if available.
    int32_t initFunctionIndex = findBytecodeFunction(fileHandle, INIT_FUNC_ID);
    if (initFunctionIndex >= 0) {
        callFunction(runningApp, runningApp, initFunctionIndex);
    }
}

void callFunction(allocPointer_t caller, allocPointer_t implementer, int32_t functionIndex) {
    
    allocPointer_t fileHandle = getRunningAppMember(implementer, fileHandle);
    int8_t fileType = getFileHandleType(fileHandle);
    allocPointer_t previousLocalFrame = getRunningAppMember(caller, localFrame);
    
    // Determine local frame size.
    heapMemoryOffset_t localFrameSize = sizeof(localFrameHeader_t);
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        localFrameSize += sizeof(bytecodeLocalFrameHeader_t) + (heapMemoryOffset_t)getBytecodeFunctionMember(fileHandle, functionIndex, localFrameSize);
    } else {
        // TODO: Determine size of local frame for system application.
        
    }
    
    // Create allocation for the local frame.
    allocPointer_t localFrame = createAlloc(LOCAL_FRAME_ALLOC_TYPE, localFrameSize);
    setLocalFrameMember(localFrame, implementer, implementer);
    setLocalFrameMember(localFrame, caller, caller);
    setLocalFrameMember(localFrame, functionIndex, functionIndex);
    setLocalFrameMember(localFrame, previousLocalFrame, previousLocalFrame);
    setLocalFrameMember(localFrame, nextArgFrame, NULL_ALLOC_POINTER);
    
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        // Initialize members specific to bytecode functions.
        int32_t instructionBodyFilePos = getBytecodeFunctionMember(
            fileHandle,
            functionIndex,
            instructionBodyFilePos
        );
        int32_t instructionBodySize = getBytecodeFunctionMember(
            fileHandle,
            functionIndex,
            instructionBodySize
        );
        setBytecodeLocalFrameMember(
            localFrame,
            instructionBodyStartFilePos,
            instructionBodyFilePos
        );
        setBytecodeLocalFrameMember(
            localFrame,
            instructionBodyEndFilePos,
            instructionBodyFilePos + instructionBodySize
        );
        setBytecodeLocalFrameMember(localFrame, instructionFilePos, instructionBodyFilePos);
        setBytecodeLocalFrameMember(localFrame, errorHandler, -1);
    }
    
    // Update caller local frame.
    setRunningAppMember(caller, localFrame, localFrame);
}

void scheduleApp(allocPointer_t runningApp) {
    
    currentLocalFrame = getRunningAppMember(runningApp, localFrame);
    if (currentLocalFrame == NULL_ALLOC_POINTER) {
        return;
    }
    
    currentImplementer = getLocalFrameMember(
        currentLocalFrame,
        implementer
    );
    currentImplementerFileHandle = getRunningAppMember(currentImplementer, fileHandle);
    int8_t fileType = getFileHandleType(currentImplementerFileHandle);
    
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        int32_t instructionBodyEndFilePos = getBytecodeLocalFrameMember(
            currentLocalFrame,
            instructionBodyEndFilePos
        );
        currentInstructionFilePos = getBytecodeLocalFrameMember(
            currentLocalFrame,
            instructionFilePos
        );
        if (currentInstructionFilePos >= instructionBodyEndFilePos) {
            // TODO: Return to caller.
            
            return;
        }
        uint8_t opcode = readFileAndAdvance(
            currentImplementerFileHandle,
            currentInstructionFilePos,
            uint8_t
        );
        printDebugStringConstant(OPCODE_DEBUG_TEXT);
        printDebugNumber(opcode);
        printDebugNewline();
        uint8_t opcodeCategory = opcode >> 4;
        uint8_t opcodeOffset = opcode & 0x0F;
        int8_t tempOffset = readArrayConstantValue(argumentAmountOffsetArray, opcodeCategory);
        int8_t argumentAmount = readArrayConstantValue(
            argumentAmountArray,
            tempOffset + opcodeOffset
        );
        for (int8_t index = 0; index < argumentAmount; index++) {
            instructionArgArray[index] = readInstructionArg();
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
                int32_t tempValue = getArgValue(1);
                setArgValue(0, tempValue);
            } else if (opcodeOffset == 0x3) {
                // newAlloc.
                int8_t isGuarded = (int8_t)getArgValue(1);
                heapMemoryOffset_t tempSize = (heapMemoryOffset_t)getArgValue(2);
                allocPointer_t tempAlloc = createDynamicAlloc(
                    tempSize,
                    isGuarded,
                    currentImplementerFileHandle
                );
                setArgValue(0, tempAlloc);
            }
        } else if (opcodeCategory == 0x1) {
            // Control flow instructions.
            if (opcodeOffset == 0x0) {
                // jmp.
                int32_t instructionOffset = getArgValue(0);
                jumpToBytecodeInstruction(currentLocalFrame, instructionOffset);
            } else if (opcodeOffset == 0x1) {
                // jmpZ.
                int32_t condition = getArgValue(1);
                if (condition == 0) {
                    int32_t instructionOffset = getArgValue(0);
                    jumpToBytecodeInstruction(currentLocalFrame, instructionOffset);
                }
            } else if (opcodeOffset == 0x2) {
                // jmpNZ.
                int32_t condition = getArgValue(1);
                if (condition != 0) {
                    int32_t instructionOffset = getArgValue(0);
                    jumpToBytecodeInstruction(currentLocalFrame, instructionOffset);
                }
            }
        } else if (opcodeCategory == 0x4) {
            // Bitwise instructions.
            uint32_t result = 0;
            if (opcodeOffset == 0x0) {
                // bNot.
                uint32_t operand = getArgValue(1);
                result = ~operand;
            } else {
                uint32_t operand1 = getArgValue(1);
                uint32_t operand2 = getArgValue(2);
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
            setArgValue(0, result);
        } else if (opcodeCategory == 0x5) {
            // Comparison instructions.
            int32_t operand1 = getArgValue(1);
            int32_t operand2 = getArgValue(2);
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
            setArgValue(0, result);
        } else if (opcodeCategory == 0x6) {
            // Arithmetic instructions.
            int32_t operand1 = getArgValue(1);
            int32_t operand2 = getArgValue(2);
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
                // TODO: Throw error when dividing by zero.
                result = operand1 / operand2;
            } else if (opcodeOffset == 0x4) {
                // mod.
                // TODO: Throw error when dividing by zero.
                result = operand1 % operand2;
            }
            setArgValue(0, result);
        }
    } else {
        // TODO: Perform work in system app.
        
    }
}


