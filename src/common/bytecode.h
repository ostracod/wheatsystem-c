
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
typedef struct bytecodeAppHeader {
    int32_t globalFrameSize;
    int32_t functionTableLength;
    int32_t appDataPos;
} bytecodeAppHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct bytecodeFunction {
    int32_t functionId;
    int8_t isGuarded;
    int32_t argumentFrameSize;
    int32_t localFrameSize;
    int32_t instructionBodyFilePos;
    int32_t instructionBodySize;
} bytecodeFunction_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct bytecodeGlobalFrameHeader {
    int32_t functionTableLength;
    int32_t appDataPos;
} bytecodeGlobalFrameHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct bytecodeLocalFrameHeader {
    int32_t instructionBodyStartFilePos;
    int32_t instructionBodyEndFilePos;
    int32_t instructionFilePos;
    int32_t errorHandler;
} bytecodeLocalFrameHeader_t;
#pragma pack(pop)

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

#pragma pack(push, 1)
typedef struct argParseContext {
    allocPointer_t localFrame;
    allocPointer_t implementer;
    allocPointer_t fileHandle;
    int32_t instructionFilePos;
} argParseContext_t;
#pragma pack(pop)

instructionArg_t instructionArgArray[MAXIMUM_ARG_AMOUNT];

#define getBytecodeAppMember(fileHandle, memberName) \
    !!!readStructMember fileHandle readFile bytecodeAppHeader_t memberName

#define readBytecodeFunctionTable(fileHandle, index, type) \
    readFile(fileHandle, sizeof(bytecodeAppHeader_t) + index, type)

#define getBytecodeFunctionMember(fileHandle, functionIndex, memberName) \
    !!!readStructMemberInTable fileHandle readBytecodeFunctionTable functionIndex bytecodeFunction_t memberName

#define getBytecodeGlobalFrameMember(runningApp, memberName) \
    !!!readStructMember runningApp readGlobalFrame bytecodeGlobalFrameHeader_t memberName
#define setBytecodeGlobalFrameMember(runningApp, memberName, value) \
    !!!writeStructMember runningApp writeGlobalFrame bytecodeGlobalFrameHeader_t memberName value
#define getBytecodeGlobalFrameDataAddress(runningApp) \
    (getGlobalFrameDataAddress(runningApp) + sizeof(bytecodeGlobalFrameHeader_t))

#define getBytecodeLocalFrameMember(localFrame, memberName) \
    !!!readStructMember localFrame readLocalFrame bytecodeLocalFrameHeader_t memberName
#define setBytecodeLocalFrameMember(localFrame, memberName, value) \
    !!!writeStructMember localFrame writeLocalFrame bytecodeLocalFrameHeader_t memberName value
#define getBytecodeLocalFrameDataAddress(localFrame) \
    (getLocalFrameDataAddress(localFrame) + sizeof(bytecodeLocalFrameHeader_t))

int32_t findBytecodeFunction(allocPointer_t fileHandle, int32_t functionId);
instructionArg_t readInstructionArg(argParseContext_t *context);


