
///DESC This file provides common definitions for running system applications. Every platform shares the same implementation of these definitions.

///FUNC
    ///RET systemApp_t
    ///DESC Convenience function to create a system application.
    ///ARG globalFrameSize
        ///TYPE int8_t
    ///ARG systemAppFunctionArray
        ///TYPE arrayConstant_t(systemAppFunction_t)
#define createSystemApp(globalFrameSize, systemAppFunctionArray) (systemApp_t){globalFrameSize, systemAppFunctionArray, getArrayConstantLength(systemAppFunctionArray)}

///FUNC
    ///RET :Type of `memberName` in `systemApp_t`
    ///DESC Retrieves a member from the system application with the given ID.
    ///ARG id
        ///TYPE int8_t
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `systemApp_t`.
#define getSystemAppMember(id, memberName) \
    readArrayStructByPointer(systemAppArray, readArrayConstantValue, id, systemApp_t, memberName)
///FUNC
    ///RET :Type of `memberName` in `systemAppFunction_t`
    ///DESC Retrieves a member from a function in the given array of system application functions.
    ///ARG systemAppFunctionArray
        ///TYPE arrayConstant_t(systemAppFunction_t)
    ///ARG index
        ///TYPE int8_t
        ///DESC Index of function in `systemAppFunctionArray`.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `systemAppFunction_t`.
#define getSystemAppFunctionListMember(systemAppFunctionArray, index, memberName) \
    readArrayStructByPointer(systemAppFunctionArray, readArrayConstantValue, index, systemAppFunction_t, memberName)
///FUNC
    ///RET :Type of `memberName` in `systemAppFunction_t`
    ///DESC Retrieves a member from a function belonging to the system application with the given ID.
    ///ARG id
        ///TYPE int8_t
    ///ARG index
        ///TYPE int8_t
        ///DESC Index of function in `functionList` of `systemApp_t`.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `systemAppFunction_t`.
#define getSystemAppFunctionMember(id, index, memberName) ({ \
    arrayConstant_t(systemAppFunction_t) functionList = getSystemAppMember(id, functionList); \
    getSystemAppFunctionListMember(functionList, index, memberName); \
})

///FUNC
    ///RET :Type of `memberName` in `systemGlobalFrameHeader_t`
    ///DESC Retrieves a member from the global frame header belonging to the given system application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
        ///DESC Running system application.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `systemGlobalFrameHeader_t`.
#define getSystemGlobalFrameMember(runningApp, memberName) \
    readStructByPointer(runningApp, readGlobalFrame, systemGlobalFrameHeader_t, memberName)
///FUNC
    ///RET void
    ///DESC Modifies a member in the global frame header belonging to the given system application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
        ///DESC Running system application.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `systemGlobalFrameHeader_t`.
    ///ARG value
        ///TYPE :Type of `memberName` in `systemGlobalFrameHeader_t`
        ///DESC Value to write.
#define setSystemGlobalFrameMember(runningApp, memberName, value) \
    writeStructByPointer(runningApp, writeGlobalFrame, systemGlobalFrameHeader_t, memberName, value)

///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the address of the global frame data region belonging to the given system application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
        ///DESC Running system application.
#define getSystemGlobalFrameDataAddress(runningApp) \
    (getGlobalFrameDataAddress(runningApp) + sizeof(systemGlobalFrameHeader_t))

///FUNC
    ///RET type
    ///DESC Reads a value from the global frame data region belonging to the given system application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
        ///DESC Running system application.
    ///ARG index
        ///TYPE heapMemoryOffset_t
        ///DESC Offset of first byte to read.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to read.
#define readSystemGlobalFrame(runningApp, index, type) \
    readHeapMemory(getSystemGlobalFrameDataAddress(runningApp) + index, type)
///FUNC
    ///RET void
    ///DESC Writes a value to the global frame data region belonging to the given system application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
        ///DESC Running system application.
    ///ARG index
        ///TYPE heapMemoryOffset_t
        ///DESC Offset of first byte to write.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to write.
    ///ARG value
        ///TYPE type
        ///DESC Value to write.
#define writeSystemGlobalFrame(runningApp, index, type, value) \
    writeHeapMemory(getSystemGlobalFrameDataAddress(runningApp) + index, type, value)

///FUNC
    ///RET :Type of `memberName` in `structDefinition`
    ///DESC Reads a global variable of the currently active system application.
    ///ARG structDefinition
        ///TYPE :Struct type
        ///DESC Structure of global frame data region.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `structDefinition`.
#define readSystemAppGlobalVariable(structDefinition, memberName) \
    readStructByPointer(currentImplementer, readSystemGlobalFrame, structDefinition, memberName)
///FUNC
    ///RET void
    ///DESC Writes a global variable of the currently active system application.
    ///ARG structDefinition
        ///TYPE :Struct type
        ///DESC Structure of global frame data region.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `structDefinition`.
    ///ARG value
        ///TYPE :Type of `memberName` in `structDefinition`
        ///DESC Value to write.
#define writeSystemAppGlobalVariable(structDefinition, memberName, value) \
    writeStructByPointer(currentImplementer, writeSystemGlobalFrame, structDefinition, memberName, value)

///FUNC
    ///RET :Type of `memberName` in `systemApp_t`
    ///DESC Retrieves a member from the given system application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
        ///DESC Running system application.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `systemApp_t`.
#define getRunningSystemAppMember(runningApp, memberName) ({ \
    int8_t systemAppId = getSystemGlobalFrameMember(runningApp, id); \
    getSystemAppMember(systemAppId, memberName); \
})
///FUNC
    ///RET :Type of `memberName` in `systemAppFunction_t`
    ///DESC Retrieves a member from a function belonging to the given system application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
        ///DESC Running system application.
    ///ARG functionIndex
        ///TYPE int8_t
        ///DESC Index of function in `functionList` of `systemApp_t`.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `systemAppFunction_t`.
#define getRunningSystemAppFunctionMember(runningApp, functionIndex, memberName) ({ \
    int8_t systemAppId = getSystemGlobalFrameMember(runningApp, id); \
    getSystemAppFunctionMember(systemAppId, functionIndex, memberName); \
})


