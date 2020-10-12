
#include "./headers.h"

declareArrayConstantWithValue(BOOT_STRING_CONSTANT, "boot");
declareArrayConstantWithValue(OPCODE_DEBUG_TEXT, "Opcode: ");

int32_t findFunctionById(allocPointer_t fileHandle, int32_t functionId) {
    int8_t fileType = getFileHandleType(fileHandle);
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        int32_t functionTableLength = getBytecodeAppMember(fileHandle, functionTableLength);
        for (int32_t index = 0; index < functionTableLength; index++) {
            int32_t tempFunctionId = getBytecodeFunctionMember(fileHandle, index, functionId);
            if (tempFunctionId == functionId) {
                return index;
            }
        }
    } else if (fileType == SYSTEM_APP_FILE_TYPE) {
        // TODO: Find function in system application.
    } else {
        // TODO: Throw an error.
    }
    return -1;
}

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
    int32_t initFunctionIndex = findFunctionById(fileHandle, INIT_FUNC_ID);
    if (initFunctionIndex >= 0) {
        callFunction(runningApp, NULL_ALLOC_POINTER, runningApp, initFunctionIndex);
    }
}

void callFunction(
    allocPointer_t threadApp,
    allocPointer_t caller,
    allocPointer_t implementer,
    int32_t functionIndex
) {
    
    allocPointer_t fileHandle = getRunningAppMember(implementer, fileHandle);
    int8_t fileType = getFileHandleType(fileHandle);
    allocPointer_t previousLocalFrame = getRunningAppMember(threadApp, localFrame);
    
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
    
    // Update thread app local frame.
    setRunningAppMember(threadApp, localFrame, localFrame);
}

void cleanUpNextArgFrame() {
    allocPointer_t nextArgFrame = getLocalFrameMember(currentLocalFrame, nextArgFrame);
    if (nextArgFrame != NULL_ALLOC_POINTER) {
        deleteAlloc(nextArgFrame);
    }
}

void returnFromFunction() {
    cleanUpNextArgFrame();
    allocPointer_t previousLocalFrame = getLocalFrameMember(
        currentLocalFrame,
        previousLocalFrame
    );
    deleteAlloc(currentLocalFrame);
    setRunningAppMember(currentThreadApp, localFrame, previousLocalFrame);
}

void scheduleAppThread(allocPointer_t runningApp) {
    
    currentThreadApp = runningApp;
    currentLocalFrame = getRunningAppMember(currentThreadApp, localFrame);
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
            returnFromFunction();
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
            instructionArgArray[index] = parseInstructionArg();
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
                    writeArgIntHelper(destination, offset, SIGNED_INT_8_TYPE, tempValue);
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
                jumpToBytecodeInstruction(currentLocalFrame, instructionOffset);
            } else if (opcodeOffset == 0x1) {
                // jmpZ.
                int32_t condition = readArgInt(1);
                if (condition == 0) {
                    int32_t instructionOffset = readArgInt(0);
                    jumpToBytecodeInstruction(currentLocalFrame, instructionOffset);
                }
            } else if (opcodeOffset == 0x2) {
                // jmpNZ.
                int32_t condition = readArgInt(1);
                if (condition != 0) {
                    int32_t instructionOffset = readArgInt(0);
                    jumpToBytecodeInstruction(currentLocalFrame, instructionOffset);
                }
            }
        } else if (opcodeCategory == 0x3) {
            // Function instructions.
            if (opcodeOffset == 0x0) {
                // findFunc.
                allocPointer_t appHandle = (allocPointer_t)readArgInt(1);
                int32_t functionId = readArgInt(2);
                int32_t functionIndex = findFunctionById(appHandle, functionId);
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
                // TODO: Throw error when dividing by zero.
                result = operand1 / operand2;
            } else if (opcodeOffset == 0x4) {
                // mod.
                // TODO: Throw error when dividing by zero.
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
        } else if (opcodeCategory == 0x9) {
            // File handle instructions.
            if (opcodeOffset == 0x0) {
                // openFile.
                allocPointer_t fileName = readArgInt(1);
                allocPointer_t fileHandle = openFileByStringAlloc(fileName);
                writeArgInt(0, fileHandle);
            } else if (opcodeOffset == 0x1) {
                // closeFile.
                allocPointer_t fileHandle = readArgInt(0);
                closeFile(fileHandle);
            }
        }
    } else {
        // TODO: Perform work in system app.
        
    }
}

void runAppSystem() {
    
    // Launch boot application.
    allocPointer_t bootFileName = createStringAllocFromArrayConstant(BOOT_STRING_CONSTANT);
    allocPointer_t bootFileHandle = openFileByStringAlloc(bootFileName);
    deleteAlloc(bootFileName);
    if (bootFileHandle == NULL_ALLOC_POINTER) {
        return;
    }
    launchApp(bootFileHandle);
    
    // Enter loop scheduling app threads.
    int8_t runningAppIndex = 0;
    while (true) {
        
        // Find running app with index equal to runningAppIndex.
        allocPointer_t runningApp = NULL_ALLOC_POINTER;
        allocPointer_t firstRunningApp = NULL_ALLOC_POINTER;
        int8_t index = 0;
        allocPointer_t tempAlloc = getFirstAlloc();
        while (tempAlloc != NULL_ALLOC_POINTER) {
            int8_t tempType = getAllocType(tempAlloc);
            if (tempType == RUNNING_APP_ALLOC_TYPE) {
                if (index == 0) {
                    firstRunningApp = tempAlloc;
                }
                if (index == runningAppIndex) {
                    runningApp = tempAlloc;
                    break;
                }
                index += 1;
            }
            tempAlloc = getAllocNext(tempAlloc);
        }
        
        // If we couldn't find running app at runningAppIndex,
        // try to schedule the first running app.
        if (runningApp == NULL_ALLOC_POINTER) {
            if (firstRunningApp == NULL_ALLOC_POINTER) {
                return;
            }
            runningApp = firstRunningApp;
            runningAppIndex = 0;
        }
        
        // Schedule thread time for runningApp.
        scheduleAppThread(runningApp);
        sleepMilliseconds(100);
        runningAppIndex += 1;
    }
}


