
#pragma pack(push, 1)
typedef struct systemAppFunction {
    int8_t id;
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
    !!!readArrayStructByPointer systemAppArray readArrayConstantValue id systemApp_t memberName
#define getSystemAppFunctionMember(systemAppFunctionArray, index, memberName) \
    !!!readArrayStructByPointer systemAppFunctionArray readArrayConstantValue index systemAppFunction_t memberName

#define getSystemGlobalFrameMember(runningApp, memberName) \
    !!!readStructByPointer runningApp readGlobalFrame systemGlobalFrameHeader_t memberName
#define setSystemGlobalFrameMember(runningApp, memberName, value) \
    !!!writeStructByPointer runningApp writeGlobalFrame systemGlobalFrameHeader_t memberName value

#define getRunningSystemAppFunctionMember(runningApp, functionIndex, memberName) ({ \
    int8_t systemAppId = getSystemGlobalFrameMember(runningApp, id); \
    arrayConstant_t(systemAppFunction_t) functionList = getSystemAppMember(systemAppId, functionList); \
    getSystemAppFunctionMember(functionList, functionIndex, memberName); \
})


