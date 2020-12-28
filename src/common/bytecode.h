
///DESC This file provides common definitions for running bytecode applications. Every platform shares the same implementation of these definitions.

#pragma pack(push, 1)
///STRUCT
    ///DESC Stored at the beginning of a bytecode application file.
    ///FIELD functionTableLength
        ///DESC Number of functions in the function table.
    ///FIELD appDataFilePos
        ///DESC Offset from the beginning of the file.
typedef struct bytecodeAppHeader_t {
    int32_t globalFrameSize;
    int32_t functionTableLength;
    int32_t appDataFilePos;
} bytecodeAppHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
///STRUCT
    ///DESC Stored in the function table of a bytecode application file.
    ///FIELD instructionBodyFilePos
        ///DESC Offset from the beginning of the file.
typedef struct bytecodeFunction_t {
    int32_t functionId;
    int8_t isGuarded;
    int32_t argFrameSize;
    int32_t localFrameSize;
    int32_t instructionBodyFilePos;
    int32_t instructionBodySize;
} bytecodeFunction_t;
#pragma pack(pop)

///STRUCT bytecodeApp_t
    ///DESC Contents of a bytecode application. Stored in the data region of a file.
    ///FIELD header
        ///TYPE bytecodeAppHeader_t
    ///FIELD functionTable
        ///TYPE :Array of `bytecodeFunction_t`
    ///FIELD instructionArray
        ///TYPE :Array of bytecode instructions
    ///FIELD appData
        ///TYPE :Array of bytes

#pragma pack(push, 1)
///STRUCT
    ///DESC Stored at the beginning of the global frame of a bytecode application.
typedef struct bytecodeGlobalFrameHeader_t {
    int32_t functionTableLength;
    int32_t appDataFilePos;
    int32_t appDataSize;
} bytecodeGlobalFrameHeader_t;
#pragma pack(pop)

///STRUCT bytecodeGlobalFrame_t
    ///DESC Contents of the global frame of a bytecode application. This struct is stored in the global frame of a `runningApp_t`.
    ///FIELD header
        ///TYPE bytecodeGlobalFrameHeader_t
    ///FIELD data
        ///TYPE :Array of bytes

#pragma pack(push, 1)
///STRUCT
    ///DESC Stored at the beginning of each local frame of a bytecode application.
    ///FIELD instructionFilePos
        ///DESC Position of current instruction to interpret.
    ///FIELD errorHandler
        ///DESC Offset from the beginning of the current function body.
typedef struct bytecodeLocalFrameHeader_t {
    int32_t instructionBodyStartFilePos;
    int32_t instructionBodyEndFilePos;
    int32_t instructionFilePos;
    int32_t errorHandler;
} bytecodeLocalFrameHeader_t;
#pragma pack(pop)

///STRUCT bytecodeLocalFrame_t
    ///DESC Contents of each local frame of a bytecode application. This struct is stored in the data region of a `localFrame_t`.
    ///FIELD header
        ///TYPE bytecodeLocalFrameHeader_t
    ///FIELD data
        ///TYPE :Array of bytes

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


