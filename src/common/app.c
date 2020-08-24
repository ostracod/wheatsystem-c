
#include "./headers.h"

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
        int32_t appDataPos = getBytecodeAppMember(fileHandle, appDataPos);
        setBytecodeGlobalFrameMember(runningApp, functionTableLength, functionTableLength);
        setBytecodeGlobalFrameMember(runningApp, appDataPos, appDataPos);
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
    heapMemoryOffset_t localFrameSize;
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        localFrameSize = sizeof(bytecodeLocalFrameHeader_t) + (heapMemoryOffset_t)getBytecodeFunctionMember(fileHandle, functionIndex, localFrameSize);
    } else {
        // TODO: Determine size of local frame for system application.
        
        localFrameSize = 0;
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
    
    allocPointer_t localFrame = getRunningAppMember(runningApp, localFrame);
    if (localFrame == NULL_ALLOC_POINTER) {
        return;
    }
    
    allocPointer_t implementer = getLocalFrameMember(localFrame, implementer);
    allocPointer_t fileHandle = getRunningAppMember(implementer, fileHandle);
    int8_t fileType = getFileHandleType(fileHandle);
    
    if (fileType == BYTECODE_APP_FILE_TYPE) {
        int32_t instructionBodyEndFilePos = getBytecodeLocalFrameMember(
            localFrame,
            instructionBodyEndFilePos
        );
        int32_t instructionFilePos = getBytecodeLocalFrameMember(
            localFrame,
            instructionFilePos
        );
        if (instructionFilePos >= instructionBodyEndFilePos) {
            // TODO: Return to caller.
            
            return;
        }
        uint8_t opcode = readFileAndAdvance(fileHandle, instructionFilePos, uint8_t);
        printf("Opcode: %d\n", opcode);
        int8_t tempOffset = readArrayConstantValue(argumentAmountOffsetArray, opcode >> 4);
        int8_t argumentAmount = readArrayConstantValue(
            argumentAmountArray,
            tempOffset + (opcode & 0x0F)
        );
        for (int8_t index = 0; index < argumentAmount; index++) {
            printf("Reading argument with index %d\n", index);
            instructionArgArray[index] = readInstructionArg(&instructionFilePos, localFrame);
        }
        setBytecodeLocalFrameMember(localFrame, instructionFilePos, instructionFilePos);
        // TODO: Perform instruction.
        
    } else {
        // TODO: Perform work in system app.
        
    }
}


