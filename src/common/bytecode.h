
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
    int32_t instructionBodyFilePos;
    int32_t instructionBodySize;
    int32_t instructionOffset;
    int32_t errorHandler;
} bytecodeLocalFrameHeader_t;
#pragma pack(pop)

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

#define getBytecodeLocalFrameMember(localFrame, memberName) \
    !!!readStructMember localFrame readLocalFrame bytecodeLocalFrameHeader_t memberName
#define setBytecodeLocalFrameMember(localFrame, memberName, value) \
    !!!writeStructMember localFrame writeLocalFrame bytecodeLocalFrameHeader_t memberName value

int32_t findBytecodeFunction(allocPointer_t fileHandle, int32_t functionId);


