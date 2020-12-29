
#include "./headers.h"

declareArrayConstantWithValue(BOOT_STRING_CONSTANT, int8_t, "boot");

allocPointer_t(runningApp_t) currentThreadApp;
allocPointer_t(localFrame_t) currentLocalFrame;
allocPointer_t(runningApp_t) currentImplementer;
allocPointer_t(fileHandle_t) currentImplementerFileHandle;
int8_t currentImplementerFileType;

int32_t findFunctionById(allocPointer_t(runningApp_t) runningApp, int32_t functionId) {
    allocPointer_t(fileHandle_t) fileHandle = getRunningAppMember(runningApp, fileHandle);
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
            int8_t tempId = getSystemAppFunctionListMember(functionList, index, id);
            if (tempId == functionId) {
                return index;
            }
        }
    }
    return -1;
}

allocPointer_t(runningApp_t) getCurrentCaller() {
    allocPointer_t(localFrame_t) previousLocalFrame = getLocalFrameMember(
        currentLocalFrame,
        previousLocalFrame
    );
    if (pointerIsNull(previousLocalFrame)) {
        return nullAllocPointer(runningApp_t);
    }
    return getLocalFrameMember(previousLocalFrame, implementer);
}

allocPointer_t(argFrame_t) createNextArgFrame(heapMemoryOffset_t size) {
    cleanUpNextArgFrame();
    allocPointer_t(argFrame_t) output = castGenericPointer(
        createAlloc(ARG_FRAME_ALLOC_TYPE, size),
        argFrame_t
    );
    setLocalFrameMember(currentLocalFrame, nextArgFrame, output);
    return output;
}

void cleanUpNextArgFrameHelper(allocPointer_t(localFrame_t) localFrame) {
    allocPointer_t(argFrame_t) nextArgFrame = getLocalFrameMember(localFrame, nextArgFrame);
    if (!pointerIsNull(nextArgFrame)) {
        deleteAlloc(nextArgFrame.genericPointer);
    }
}

void launchApp(allocPointer_t(fileHandle_t) fileHandle) {
    
    // Do not launch app again if it is already running.
    allocPointer_t(runningApp_t) runningApp = getFileHandleRunningApp(fileHandle);
    if (!pointerIsNull(runningApp)) {
        return;
    }
    int8_t fileType = getFileHandleType(fileHandle);
    int32_t fileSize = getFileHandleSize(fileHandle);
    int32_t functionTableLength;
    int32_t appDataFilePos;
    int8_t systemAppId;
    
    // Determine global frame size.
    heapMemoryOffset_t globalFrameSize;
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        
        // Validate bytecode app file.
        functionTableLength = getBytecodeAppMember(fileHandle, functionTableLength);
        appDataFilePos = getBytecodeAppMember(fileHandle, appDataFilePos);
        int32_t expectedFilePos = sizeof(bytecodeAppHeader_t) + functionTableLength * sizeof(bytecodeFunction_t);
        for (int32_t index = 0; index < functionTableLength; index++) {
            int32_t instructionBodyFilePos = getBytecodeFunctionMember(
                fileHandle,
                index,
                instructionBodyFilePos
            );
            int32_t instructionBodySize = getBytecodeFunctionMember(
                fileHandle,
                index,
                instructionBodySize
            );
            if (instructionBodyFilePos != expectedFilePos) {
                unhandledErrorCode = DATA_ERR_CODE;
                return;
            }
            expectedFilePos += instructionBodySize;
        }
        if (expectedFilePos != appDataFilePos || appDataFilePos > fileSize) {
            unhandledErrorCode = DATA_ERR_CODE;
            return;
        }
        
        globalFrameSize = sizeof(bytecodeGlobalFrameHeader_t) + (heapMemoryOffset_t)getBytecodeAppMember(fileHandle, globalFrameSize);
    } else if (fileType == SYSTEM_APP_FILE_TYPE) {
        
        // Validate system app file.
        if (fileSize != 1) {
            unhandledErrorCode = DATA_ERR_CODE;
            return;
        }
        
        systemAppId = readFile(fileHandle, 0, int8_t);
        globalFrameSize = sizeof(systemGlobalFrameHeader_t) + getSystemAppMember(systemAppId, globalFrameSize);
    } else {
        unhandledErrorCode = TYPE_ERR_CODE;
        return;
    }
    
    // Create allocation for the running app.
    runningApp = castGenericPointer(
        createAlloc(RUNNING_APP_ALLOC_TYPE, sizeof(runningAppHeader_t) + globalFrameSize),
        runningApp_t
    );
    setRunningAppMember(runningApp, fileHandle, fileHandle);
    setRunningAppMember(runningApp, localFrame, nullAllocPointer(localFrame_t));
    setRunningAppMember(runningApp, isWaiting, false);
    setFileHandleRunningApp(fileHandle, runningApp);
    setFileHandleInitErr(fileHandle, 0);
    
    // Initialize global frame.
    for (heapMemoryOffset_t index = 0; index < globalFrameSize; index++) {
        writeGlobalFrame(runningApp, index, int8_t, 0);
    }
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        setBytecodeGlobalFrameMember(runningApp, functionTableLength, functionTableLength);
        setBytecodeGlobalFrameMember(runningApp, appDataFilePos, appDataFilePos);
        setBytecodeGlobalFrameMember(runningApp, appDataSize, fileSize - appDataFilePos);
    } else {
        setSystemGlobalFrameMember(runningApp, id, systemAppId);
    }
    
    // Call init function if available.
    int32_t initFunctionIndex = findFunctionById(runningApp, INIT_FUNC_ID);
    if (initFunctionIndex >= 0) {
        callFunction(runningApp, runningApp, initFunctionIndex);
    }
}

void hardKillApp(allocPointer_t(runningApp_t) runningApp, int8_t errorCode) {
    
    // Delete local frames and argument frames.
    allocPointer_t(localFrame_t) localFrame = getRunningAppMember(runningApp, localFrame);
    while (!pointerIsNull(localFrame)) {
        cleanUpNextArgFrameHelper(localFrame);
        allocPointer_t(localFrame_t) previousLocalFrame = getLocalFrameMember(
            localFrame,
            previousLocalFrame
        );
        deleteAlloc(localFrame.genericPointer);
        localFrame = previousLocalFrame;
    }
    
    // Delete dynamic allocations.
    allocPointer_t(fileHandle_t) fileHandle = getRunningAppMember(
        currentThreadApp,
        fileHandle
    );
    genericAllocPointer_t tempAlloc = getFirstAlloc();
    while (tempAlloc != NULL_ALLOC_POINTER) {
        genericAllocPointer_t nextAlloc = getAllocNext(tempAlloc);
        int8_t tempType = getAllocType(tempAlloc);
        if (tempType == DYNAMIC_ALLOC_TYPE) {
            allocPointer_t(fileHandle_t) tempCreator = getDynamicAllocMember(
                castGenericPointer(tempAlloc, dynamicAlloc_t),
                creator
            );
            if (pointersAreEqual(tempCreator, fileHandle)) {
                deleteAlloc(tempAlloc);
            }
        }
        tempAlloc = nextAlloc;
    }
    
    // Update file handle and delete running app.
    setFileHandleRunningApp(fileHandle, nullAllocPointer(runningApp_t));
    setFileHandleInitErr(fileHandle, errorCode);
    closeFile(fileHandle);
    deleteAlloc(runningApp.genericPointer);
}

void callFunction(
    allocPointer_t(runningApp_t) threadApp,
    allocPointer_t(runningApp_t) implementer,
    int32_t functionIndex
) {
    
    allocPointer_t(fileHandle_t) fileHandle = getRunningAppMember(implementer, fileHandle);
    int8_t fileType = getFileHandleType(fileHandle);
    allocPointer_t(localFrame_t) previousLocalFrame = getRunningAppMember(
        threadApp,
        localFrame
    );
    arrayConstant_t(systemAppFunction_t) systemAppFunctionList;
    
    // Validate function index.
    int32_t functionAmount;
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        functionAmount = getBytecodeGlobalFrameMember(implementer, functionTableLength);
    } else {
        int8_t systemAppId = getSystemGlobalFrameMember(implementer, id);
        systemAppFunctionList = getSystemAppMember(systemAppId, functionList);
        functionAmount = getSystemAppMember(systemAppId, functionAmount);
    }
    if (functionIndex < 0 || functionIndex >= functionAmount) {
        unhandledErrorCode = INDEX_ERR_CODE;
        return;
    }
    
    // Validate arg frame size.
    int32_t argFrameSize;
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        argFrameSize = getBytecodeFunctionMember(fileHandle, functionIndex, argFrameSize);
    } else {
        argFrameSize = getSystemAppFunctionListMember(
            systemAppFunctionList,
            functionIndex,
            argFrameSize
        );
    }
    if (argFrameSize > 0) {
        allocPointer_t(argFrame_t) previousArgFrame;
        if (pointerIsNull(previousLocalFrame)) {
            previousArgFrame = nullAllocPointer(argFrame_t);
        } else {
            previousArgFrame = getLocalFrameMember(previousLocalFrame, nextArgFrame);
        }
        if (pointerIsNull(previousArgFrame)
                || getArgFrameSize(previousArgFrame) != argFrameSize) {
            unhandledErrorCode = ARG_FRAME_ERR_CODE;
            return;
        }
    }
    
    // Determine local frame size.
    heapMemoryOffset_t localFrameSize = sizeof(localFrameHeader_t);
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        localFrameSize += sizeof(bytecodeLocalFrameHeader_t) + (heapMemoryOffset_t)getBytecodeFunctionMember(fileHandle, functionIndex, localFrameSize);
    } else {
        localFrameSize += getSystemAppFunctionListMember(
            systemAppFunctionList,
            functionIndex,
            localFrameSize
        );
    }
    
    // Create allocation for the local frame.
    allocPointer_t(localFrame_t) localFrame = castGenericPointer(
        createAlloc(LOCAL_FRAME_ALLOC_TYPE, localFrameSize),
        localFrame_t
    );
    setLocalFrameMember(localFrame, implementer, implementer);
    setLocalFrameMember(localFrame, functionIndex, functionIndex);
    setLocalFrameMember(localFrame, previousLocalFrame, previousLocalFrame);
    setLocalFrameMember(localFrame, nextArgFrame, nullAllocPointer(argFrame_t));
    
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
void setCurrentLocalFrame(allocPointer_t(localFrame_t) localFrame) {
    currentLocalFrame = localFrame;
    currentImplementer = getLocalFrameMember(currentLocalFrame, implementer);
    currentImplementerFileHandle = getRunningAppMember(currentImplementer, fileHandle);
    currentImplementerFileType = getFileHandleType(currentImplementerFileHandle);
}

void returnFromFunction() {
    cleanUpNextArgFrame();
    allocPointer_t(localFrame_t) previousLocalFrame = getLocalFrameMember(
        currentLocalFrame,
        previousLocalFrame
    );
    deleteAlloc(currentLocalFrame.genericPointer);
    setRunningAppMember(currentThreadApp, localFrame, previousLocalFrame);
    setCurrentLocalFrame(previousLocalFrame);
}

void scheduleAppThread(allocPointer_t(runningApp_t) runningApp) {
    
    currentThreadApp = runningApp;
    allocPointer_t(localFrame_t) tempFrame = getRunningAppMember(
        currentThreadApp,
        localFrame
    );
    if (pointerIsNull(tempFrame)) {
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
        //printDebugString((int8_t *)"UNHANDLED ERROR ");
        //printDebugNumber(unhandledErrorCode);
        //printDebugNewline();
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
            if (pointerIsNull(currentLocalFrame)) {
                hardKillApp(currentThreadApp, unhandledErrorCode);
                break;
            }
        }
        unhandledErrorCode = 0;
    }
}

void runAppSystem() {
    
    // Launch boot application.
    allocPointer_t(dynamicAlloc_t) bootFileName = createStringAllocFromArrayConstant(
        BOOT_STRING_CONSTANT
    );
    allocPointer_t(fileHandle_t) bootFileHandle = openFileByStringAlloc(bootFileName);
    deleteAlloc(bootFileName.genericPointer);
    if (pointerIsNull(bootFileHandle)) {
        return;
    }
    launchApp(bootFileHandle);
    if (unhandledErrorCode != 0) {
        return;
    }
    
    // Enter loop scheduling app threads.
    int8_t runningAppIndex = 0;
    while (true) {
        
        // Find running app with index equal to runningAppIndex.
        allocPointer_t(runningApp_t) runningApp = nullAllocPointer(runningApp_t);
        allocPointer_t(runningApp_t) firstRunningApp = nullAllocPointer(runningApp_t);
        int8_t index = 0;
        genericAllocPointer_t tempAlloc = getFirstAlloc();
        while (tempAlloc != NULL_ALLOC_POINTER) {
            int8_t tempType = getAllocType(tempAlloc);
            if (tempType == RUNNING_APP_ALLOC_TYPE) {
                allocPointer_t(runningApp_t) tempRunningApp = castGenericPointer(
                    tempAlloc,
                    runningApp_t
                );
                if (index == 0) {
                    firstRunningApp = tempRunningApp;
                }
                if (index == runningAppIndex) {
                    runningApp = tempRunningApp;
                    break;
                }
                index += 1;
            }
            tempAlloc = getAllocNext(tempAlloc);
        }
        
        // If we couldn't find running app at runningAppIndex,
        // try to schedule the first running app.
        if (pointerIsNull(runningApp)) {
            if (pointerIsNull(firstRunningApp)) {
                return;
            }
            runningApp = firstRunningApp;
            runningAppIndex = 0;
        }
        
        // Schedule thread time for runningApp.
        scheduleAppThread(runningApp);
        sleepMilliseconds(1);
        runningAppIndex += 1;
    }
}


