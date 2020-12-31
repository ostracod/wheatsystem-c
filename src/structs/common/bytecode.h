
///DESC This file provides common data structures for bytecode applications. Every platform shares the same implementation of these definitions.

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


