
#include "./headers.h"

declareArrayConstantWithValue(BOOT_STRING_CONSTANT, int8_t, "boot");

allocPointer_t currentThreadApp;
allocPointer_t currentLocalFrame;
allocPointer_t currentImplementer;
allocPointer_t currentImplementerFileHandle;
int8_t currentImplementerFileType;

int32_t findFunctionById(allocPointer_t runningApp, int32_t functionId) {
    allocPointer_t fileHandle = getRunningAppMember(runningApp, fileHandle);
    int8_t fileType = getFileHandleType(fileHandle);
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        int32_t functionTableLength = getBytecodeGlobalFrameMember(
            runningApp,
            functionTableLength
        );
        for (int32_t index = 0; index < functionTableLength; index++) {
            int32_t tempFunctionId = getBytecodeFunctionMember(fileHandle, index, functionId);
            if (tempFunctionId == functionId) {
                return index;
            }
        }
    } else if (fileType == SYSTEM_APP_FILE_TYPE) {
        int8_t systemAppId = getSystemGlobalFrameMember(runningApp, id);
        arrayConstant_t(systemAppFunction_t) functionList = getSystemAppMember(
            systemAppId,
            functionList
        );
        int8_t functionAmount = getSystemAppMember(systemAppId, functionAmount);
        for (int8_t index = 0; index < functionAmount; index++) {
            int8_t tempId = getSystemAppFunctionMember(functionList, index, id);
            if (tempId == functionId) {
                return index;
            }
        }
    }
    return -1;
}

void cleanUpNextArgFrameHelper(allocPointer_t localFrame) {
    allocPointer_t nextArgFrame = getLocalFrameMember(localFrame, nextArgFrame);
    if (nextArgFrame != NULL_ALLOC_POINTER) {
        deleteAlloc(nextArgFrame);
    }
}

void launchApp(allocPointer_t fileHandle) {
    
    // Do not launch app again if it is already running.
    allocPointer_t runningApp = getFileHandleRunningApp(fileHandle);
    if (runningApp != NULL_ALLOC_POINTER) {
        return;
    }
    int8_t fileType = getFileHandleType(fileHandle);
    int8_t systemAppId;
    
    // Determine global frame size.
    heapMemoryOffset_t globalFrameSize;
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        globalFrameSize = sizeof(bytecodeGlobalFrameHeader_t) + (heapMemoryOffset_t)getBytecodeAppMember(fileHandle, globalFrameSize);
    } else if (fileType == SYSTEM_APP_FILE_TYPE) {
        systemAppId = readFile(fileHandle, 0, int8_t);
        globalFrameSize = sizeof(systemGlobalFrameHeader_t) + getSystemAppMember(systemAppId, globalFrameSize);
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
    setFileHandleInitErr(fileHandle, 0);
    
    // Initialize global frame.
    for (heapMemoryOffset_t index = 0; index < globalFrameSize; index++) {
        writeGlobalFrame(runningApp, index, int8_t, 0);
    }
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        int32_t functionTableLength = getBytecodeAppMember(fileHandle, functionTableLength);
        int32_t appDataFilePos = getBytecodeAppMember(fileHandle, appDataFilePos);
        int32_t appDataSize = getFileHandleSize(fileHandle) - appDataFilePos;
        setBytecodeGlobalFrameMember(runningApp, functionTableLength, functionTableLength);
        setBytecodeGlobalFrameMember(runningApp, appDataFilePos, appDataFilePos);
        setBytecodeGlobalFrameMember(runningApp, appDataSize, appDataSize);
    } else {
        setSystemGlobalFrameMember(runningApp, id, systemAppId);
    }
    
    // Call init function if available.
    int32_t initFunctionIndex = findFunctionById(runningApp, INIT_FUNC_ID);
    if (initFunctionIndex >= 0) {
        callFunction(runningApp, NULL_ALLOC_POINTER, runningApp, initFunctionIndex);
    }
}

void hardKillApp(allocPointer_t runningApp, int8_t errorCode) {
    
    // Delete local frames and argument frames.
    allocPointer_t localFrame = getRunningAppMember(runningApp, localFrame);
    while (localFrame != NULL_ALLOC_POINTER) {
        cleanUpNextArgFrameHelper(localFrame);
        allocPointer_t previousLocalFrame = getLocalFrameMember(
            localFrame,
            previousLocalFrame
        );
        deleteAlloc(localFrame);
        localFrame = previousLocalFrame;
    }
    
    // Delete dynamic allocations.
    allocPointer_t fileHandle = getRunningAppMember(currentThreadApp, fileHandle);
    allocPointer_t tempAlloc = getFirstAlloc();
    while (tempAlloc != NULL_ALLOC_POINTER) {
        allocPointer_t nextAlloc = getAllocNext(tempAlloc);
        int8_t tempType = getAllocType(tempAlloc);
        if (tempType == DYNAMIC_ALLOC_TYPE) {
            allocPointer_t tempCreator = getDynamicAllocMember(tempAlloc, creator);
            if (tempCreator == fileHandle) {
                deleteAlloc(tempAlloc);
            }
        }
        tempAlloc = nextAlloc;
    }
    
    // Update file handle and delete running app.
    setFileHandleRunningApp(fileHandle, NULL_ALLOC_POINTER);
    setFileHandleInitErr(fileHandle, errorCode);
    closeFile(fileHandle);
    deleteAlloc(runningApp);
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
        localFrameSize += getRunningSystemAppFunctionMember(
            implementer,
            functionIndex,
            localFrameSize
        );
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

// Does not update currentThreadApp.
void setCurrentLocalFrame(allocPointer_t localFrame) {
    currentLocalFrame = localFrame;
    currentImplementer = getLocalFrameMember(currentLocalFrame, implementer);
    currentImplementerFileHandle = getRunningAppMember(currentImplementer, fileHandle);
    currentImplementerFileType = getFileHandleType(currentImplementerFileHandle);
}

void returnFromFunction() {
    cleanUpNextArgFrame();
    allocPointer_t previousLocalFrame = getLocalFrameMember(
        currentLocalFrame,
        previousLocalFrame
    );
    deleteAlloc(currentLocalFrame);
    setRunningAppMember(currentThreadApp, localFrame, previousLocalFrame);
    setCurrentLocalFrame(previousLocalFrame);
}

void scheduleAppThread(allocPointer_t runningApp) {
    
    currentThreadApp = runningApp;
    int8_t tempFrame = getRunningAppMember(currentThreadApp, localFrame);
    if (tempFrame == NULL_ALLOC_POINTER) {
        return;
    }
    setCurrentLocalFrame(tempFrame);
    
    if (currentImplementerFileType == BYTECODE_APP_FILE_TYPE) {
        evaluateBytecodeInstruction();
    } else {
        int8_t functionIndex = (int8_t)getLocalFrameMember(currentLocalFrame, functionIndex);
        void (*threadAction)() = getRunningSystemAppFunctionMember(
            currentImplementer,
            functionIndex,
            threadAction
        );
        threadAction();
    }
    
    if (unhandledErrorCode != 0) {
        printDebugString((int8_t *)"UNHANDLED ERROR ");
        printDebugNumber(unhandledErrorCode);
        printDebugNewline();
        while (true) {
            int8_t shouldHandleError;
            if (currentImplementerFileType == BYTECODE_APP_FILE_TYPE) {
                int32_t instructionOffset = getBytecodeLocalFrameMember(
                    currentLocalFrame,
                    errorHandler
                );
                if (instructionOffset >= 0) {
                    jumpToBytecodeInstruction(instructionOffset);
                    shouldHandleError = true;
                } else {
                    shouldHandleError = false;
                }
            } else {
                shouldHandleError = true;
            }
            if (shouldHandleError) {
                setLocalFrameMember(currentLocalFrame, lastErrorCode, unhandledErrorCode);
                break;
            }
            returnFromFunction();
            if (currentLocalFrame == NULL_ALLOC_POINTER) {
                hardKillApp(currentThreadApp, unhandledErrorCode);
                break;
            }
        }
        unhandledErrorCode = 0;
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
        sleepMilliseconds(20);
        runningAppIndex += 1;
    }
}


