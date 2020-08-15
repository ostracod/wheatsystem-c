
#pragma pack(push, 1)
typedef struct bytecodeAppHeader {
    int32_t globalFrameSize;
    int32_t functionTableLength;
    int32_t appDataPos;
} bytecodeAppHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct bytecodeAppCache {
    int32_t functionTableLength;
    int32_t appDataPos;
} bytecodeAppCache_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct bytecodeFunction {
    int32_t functionId;
    int8_t isGuarded;
    int32_t argumentFrameSize;
    int32_t localFrameSize;
    int32_t instructionBodyFilePosition;
    int32_t instructionBodySize;
} bytecodeFunction_t;
#pragma pack(pop)

#define getBytecodeAppHeaderMember(fileHandle, memberName) \
    !!!readStructMember fileHandle readFile bytecodeAppHeader_t memberName
#define getBytecodeAppCacheMember(runningApp, memberName) \
    !!!readStructMember runningApp readGlobalFrame bytecodeAppCache_t memberName

#define readBytecodeFunctionTable(fileHandle, index, type) \
    readFile(fileHandle, sizeof(bytecodeAppHeader_t) + index, type)

#define getBytecodeFunctionMember(fileHandle, functionIndex, memberName) \
    !!!readStructMemberInTable fileHandle readBytecodeFunctionTable functionIndex bytecodeFunction_t memberName

void initializeBytecodeAppGlobalFrame(allocPointer_t runningApp);
int32_t findBytecodeFunction(allocPointer_t fileHandle, int32_t functionId);


