
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

#define getBytecodeAppHeaderMember(fileHandle, memberName) \
    readStructMember2(fileHandle, readFile, bytecodeAppHeader_t, memberName)
#define getBytecodeAppCacheMember(runningApp, memberName) \
    readStructMember2(runningApp, readGlobalFrame, bytecodeAppCache_t, memberName)

void initializeBytecodeAppGlobalFrame(allocPointer_t runningApp);


