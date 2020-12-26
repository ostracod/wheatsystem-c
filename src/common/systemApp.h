
#pragma pack(push, 1)
typedef struct systemAppFunction {
    int8_t id;
    int8_t argFrameSize;
    int8_t localFrameSize;
    void (*threadAction)();
} systemAppFunction_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct systemApp {
    int8_t globalFrameSize;
    arrayConstant_t(systemAppFunction_t) functionList;
    int8_t functionAmount;
} systemApp_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct systemGlobalFrameHeader {
    int8_t id;
} systemGlobalFrameHeader_t;
#pragma pack(pop)

#define createSystemApp(globalFrameSize, systemAppFunctionArray) (systemApp_t){globalFrameSize, systemAppFunctionArray, getArrayConstantLength(systemAppFunctionArray)}

#define getSystemAppMember(id, memberName) \
    readArrayStructByPointer(systemAppArray, readArrayConstantValue, id, systemApp_t, memberName)
#define getSystemAppFunctionListMember(systemAppFunctionArray, index, memberName) \
    readArrayStructByPointer(systemAppFunctionArray, readArrayConstantValue, index, systemAppFunction_t, memberName)
#define getSystemAppFunctionMember(id, index, memberName) ({ \
    arrayConstant_t(systemAppFunction_t) functionList = getSystemAppMember(id, functionList); \
    getSystemAppFunctionListMember(functionList, index, memberName); \
})

#define getSystemGlobalFrameMember(runningApp, memberName) \
    readStructByPointer(runningApp, readGlobalFrame, systemGlobalFrameHeader_t, memberName)
#define setSystemGlobalFrameMember(runningApp, memberName, value) \
    writeStructByPointer(runningApp, writeGlobalFrame, systemGlobalFrameHeader_t, memberName, value)

#define getSystemGlobalFrameDataAddress(runningApp) \
    (getGlobalFrameDataAddress(runningApp) + sizeof(systemGlobalFrameHeader_t))

#define readSystemGlobalFrame(runningApp, index, type) \
    readHeapMemory(getSystemGlobalFrameDataAddress(runningApp) + index, type)
#define writeSystemGlobalFrame(runningApp, index, type, value) \
    writeHeapMemory(getSystemGlobalFrameDataAddress(runningApp) + index, type, value)

#define readSystemAppGlobalVariable(structDefinition, memberName) \
    readStructByPointer(currentImplementer, readSystemGlobalFrame, structDefinition, memberName)
#define writeSystemAppGlobalVariable(structDefinition, memberName, value) \
    writeStructByPointer(currentImplementer, writeSystemGlobalFrame, structDefinition, memberName, value)

#define getRunningSystemAppMember(runningApp, memberName) ({ \
    int8_t systemAppId = getSystemGlobalFrameMember(runningApp, id); \
    getSystemAppMember(systemAppId, memberName); \
})
#define getRunningSystemAppFunctionMember(runningApp, functionIndex, memberName) ({ \
    int8_t systemAppId = getSystemGlobalFrameMember(runningApp, id); \
    getSystemAppFunctionMember(systemAppId, functionIndex, memberName); \
})


