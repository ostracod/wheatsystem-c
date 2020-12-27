
#pragma pack(push, 1)
typedef struct bytecodeAppHeader_t {
    int32_t globalFrameSize;
    int32_t functionTableLength;
    int32_t appDataFilePos;
} bytecodeAppHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct bytecodeFunction_t {
    int32_t functionId;
    int8_t isGuarded;
    int32_t argFrameSize;
    int32_t localFrameSize;
    int32_t instructionBodyFilePos;
    int32_t instructionBodySize;
} bytecodeFunction_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct bytecodeGlobalFrameHeader_t {
    int32_t functionTableLength;
    int32_t appDataFilePos;
    int32_t appDataSize;
} bytecodeGlobalFrameHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct bytecodeLocalFrameHeader_t {
    int32_t instructionBodyStartFilePos;
    int32_t instructionBodyEndFilePos;
    int32_t instructionFilePos;
    int32_t errorHandler;
} bytecodeLocalFrameHeader_t;
#pragma pack(pop)

#define getBytecodeAppMember(fileHandle, memberName) \
    readStructByPointer(fileHandle, readFile, bytecodeAppHeader_t, memberName)

#define readBytecodeFunctionTable(fileHandle, index, type) \
    readFile(fileHandle, sizeof(bytecodeAppHeader_t) + index, type)

#define getBytecodeFunctionMember(fileHandle, functionIndex, memberName) \
    readArrayStructByPointer(fileHandle, readBytecodeFunctionTable, functionIndex, bytecodeFunction_t, memberName)

#define getBytecodeGlobalFrameMember(runningApp, memberName) \
    readStructByPointer(runningApp, readGlobalFrame, bytecodeGlobalFrameHeader_t, memberName)
#define setBytecodeGlobalFrameMember(runningApp, memberName, value) \
    writeStructByPointer(runningApp, writeGlobalFrame, bytecodeGlobalFrameHeader_t, memberName, value)

#define getBytecodeGlobalFrameDataAddress(runningApp) \
    (getGlobalFrameDataAddress(runningApp) + sizeof(bytecodeGlobalFrameHeader_t))
#define getBytecodeGlobalFrameSize(runningApp) \
    (getGlobalFrameSize(runningApp) - sizeof(bytecodeGlobalFrameHeader_t))

#define getBytecodeLocalFrameMember(localFrame, memberName) \
    readStructByPointer(localFrame, readLocalFrame, bytecodeLocalFrameHeader_t, memberName)
#define setBytecodeLocalFrameMember(localFrame, memberName, value) \
    writeStructByPointer(localFrame, writeLocalFrame, bytecodeLocalFrameHeader_t, memberName, value)

#define getBytecodeLocalFrameDataAddress(localFrame) \
    (getLocalFrameDataAddress(localFrame) + sizeof(bytecodeLocalFrameHeader_t))
#define getBytecodeLocalFrameSize(localFrame) \
    (getLocalFrameSize(localFrame) - sizeof(bytecodeLocalFrameHeader_t))

void jumpToBytecodeInstruction(int32_t instructionOffset);
void evaluateBytecodeInstruction();


