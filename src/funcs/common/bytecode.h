
///DESC This file provides common functions for running bytecode applications. Every platform shares the same implementation of these definitions.

///FUNC
    ///RET :Type of `memberName` in `bytecodeAppHeader_t`
    ///DESC Retrieves a member from the header of the given bytecode application.
    ///ARG fileHandle
        ///TYPE allocPointer_t(fileHandle_t)
        ///DESC File handle of a bytecode application.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `bytecodeAppHeader_t`.
#define getBytecodeAppMember(fileHandle, memberName) \
    readStructByPointer(fileHandle, readFile, bytecodeAppHeader_t, memberName)

#define readBytecodeFunctionTable(fileHandle, index, type) \
    readFile(fileHandle, sizeof(bytecodeAppHeader_t) + index, type)

///FUNC
    ///RET :Type of `memberName` in `bytecodeFunction_t`
    ///DESC Retrieves a member from a function in the given bytecode application.
    ///ARG fileHandle
        ///TYPE allocPointer_t(fileHandle_t)
        ///DESC File handle of a bytecode application.
    ///ARG functionIndex
        ///TYPE int32_t
        ///DESC Index in the function table.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `bytecodeFunction_t`.
#define getBytecodeFunctionMember(fileHandle, functionIndex, memberName) \
    readArrayStructByPointer(fileHandle, readBytecodeFunctionTable, functionIndex, bytecodeFunction_t, memberName)

///FUNC
    ///RET :Type of `memberName` in `bytecodeGlobalFrameHeader_t`
    ///DESC Retrieves a member from the global frame header belonging to the given bytecode application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
        ///DESC Running bytecode application.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `bytecodeGlobalFrameHeader_t`.
#define getBytecodeGlobalFrameMember(runningApp, memberName) \
    readStructByPointer(runningApp, readGlobalFrame, bytecodeGlobalFrameHeader_t, memberName)
///FUNC
    ///RET void
    ///DESC Modifies a member in the global frame header belonging to the given bytecode application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
        ///DESC Running bytecode application.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `bytecodeGlobalFrameHeader_t`.
    ///ARG value
        ///TYPE :Type of `memberName` in `bytecodeGlobalFrameHeader_t`
        ///DESC Value to write.
#define setBytecodeGlobalFrameMember(runningApp, memberName, value) \
    writeStructByPointer(runningApp, writeGlobalFrame, bytecodeGlobalFrameHeader_t, memberName, value)

///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the address of the global frame data region belonging to the given bytecode application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
        ///DESC Running bytecode application.
#define getBytecodeGlobalFrameDataAddress(runningApp) \
    (getGlobalFrameDataAddress(runningApp) + sizeof(bytecodeGlobalFrameHeader_t))
///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the size of the global frame data region belonging to the given bytecode application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
        ///DESC Running bytecode application.
#define getBytecodeGlobalFrameSize(runningApp) \
    (getGlobalFrameSize(runningApp) - sizeof(bytecodeGlobalFrameHeader_t))

///FUNC
    ///RET :Type of `memberName` in `bytecodeLocalFrameHeader_t`
    ///DESC Retrieves a member from the header of the given bytecode local frame.
    ///ARG localFrame
        ///TYPE allocPointer_t(localFrame_t)
        ///DESC Local frame of a bytecode application.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `bytecodeLocalFrameHeader_t`.
#define getBytecodeLocalFrameMember(localFrame, memberName) \
    readStructByPointer(localFrame, readLocalFrame, bytecodeLocalFrameHeader_t, memberName)
///FUNC
    ///RET void
    ///DESC Modifies a member in the header of the given bytecode local frame.
    ///ARG localFrame
        ///TYPE allocPointer_t(localFrame_t)
        ///DESC Local frame of a bytecode application.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `bytecodeLocalFrameHeader_t`.
    ///ARG value
        ///TYPE :Type of `memberName` in `bytecodeLocalFrameHeader_t`
        ///DESC Value to write.
#define setBytecodeLocalFrameMember(localFrame, memberName, value) \
    writeStructByPointer(localFrame, writeLocalFrame, bytecodeLocalFrameHeader_t, memberName, value)

///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the address of the data region in the given bytecode local frame.
    ///ARG localFrame
        ///TYPE allocPointer_t(localFrame_t)
        ///DESC Local frame of a bytecode application.
#define getBytecodeLocalFrameDataAddress(localFrame) \
    (getLocalFrameDataAddress(localFrame) + sizeof(bytecodeLocalFrameHeader_t))
///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the size of the data region in the given bytecode local frame.
    ///ARG localFrame
        ///TYPE allocPointer_t(localFrame_t)
        ///DESC Local frame of a bytecode application.
#define getBytecodeLocalFrameSize(localFrame) \
    (getLocalFrameSize(localFrame) - sizeof(bytecodeLocalFrameHeader_t))

///FUNC
    ///DESC Causes the bytecode interpreter to jump to the given position in the current function.
    ///ARG instructionOffset
        ///DESC Offset from the beginning of the current function body.
void jumpToBytecodeInstruction(int32_t instructionOffset);
///FUNC
    ///DESC Interprets one bytecode instruction of the currently scheduled bytecode application.
void evaluateBytecodeInstruction();


