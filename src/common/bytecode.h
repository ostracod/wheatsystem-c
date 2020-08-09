
#pragma pack(push, 1)
typedef struct bytecodeAppCache {
    int32_t functionTableLength;
    int32_t appDataPos;
} bytecodeAppCache_t;
#pragma pack(pop)

#define getBytecodeAppCacheMember(runningApp, memberName) \
    readStructMember(runningApp, readGlobalFrame, bytecodeAppCache_t, memberName)

#define getBytecodeFunctionTableLength(runningApp) \
    getBytecodeAppCacheMember(runningApp, functionTableLength)
#define getBytecodeAppDataPos(runningApp) \
    getBytecodeAppCacheMember(runningApp, appDataPos)

void initializeBytecodeAppGlobalFrame(allocPointer_t runningApp);


