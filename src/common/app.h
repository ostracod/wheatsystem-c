
///DESC This file provides common definitions for running applications. Every platform shares the same implementation of these definitions.

///CONST
    ///TYPE int8_t
#define INIT_FUNC_ID 1
///CONST
    ///TYPE int8_t
#define KILL_FUNC_ID 2
///CONST
    ///TYPE int8_t
#define LISTEN_TERM_FUNC_ID 3
///CONST
    ///TYPE int8_t
#define TERM_SIZE_FUNC_ID 4
///CONST
    ///TYPE int8_t
#define WRT_TERM_FUNC_ID 5
///CONST
    ///TYPE int8_t
#define TERM_INPUT_FUNC_ID 6
///CONST
    ///TYPE int8_t
#define START_SERIAL_FUNC_ID 7
///CONST
    ///TYPE int8_t
#define STOP_SERIAL_FUNC_ID 8
///CONST
    ///TYPE int8_t
#define WRT_SERIAL_FUNC_ID 9
///CONST
    ///TYPE int8_t
#define SERIAL_INPUT_FUNC_ID 10
///CONST
    ///TYPE int8_t
#define SET_GPIO_MODE_FUNC_ID 11
///CONST
    ///TYPE int8_t
#define READ_GPIO_FUNC_ID 12
///CONST
    ///TYPE int8_t
#define WRT_GPIO_FUNC_ID 13

#pragma pack(push, 1)
///STRUCT
    ///DESC Stored at the beginning of a running application allocation.
    ///FIELD fileHandle
        ///DESC File handle of an application.
    ///FIELD localFrame
        ///DESC Currently active local frame.
    ///FIELD isWaiting
        ///DESC Whether the application is blocked by a `wait` instruction.
typedef struct runningAppHeader_t {
    allocPointer_t(fileHandle_t) fileHandle;
    allocPointer_t(localFrame_t) localFrame;
    int8_t isWaiting;
} runningAppHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
///STRUCT
    ///DESC Contains the state of a running application. This struct is stored in the data region of a heap allocation.
    ///FIELD globalFrameData
        ///TYPE :Array of bytes
        ///DESC Stores global variables of the application.
typedef struct runningApp_t {
    runningAppHeader_t header;
    int8_t globalFrameData[0];
} runningApp_t;
#pragma pack(pop)

#pragma pack(push, 1)
///STRUCT
    ///DESC Stored at the beginning of a local frame.
    ///FIELD implementer
        ///DESC Running application which implements the function.
    ///FIELD nextArgFrame
        ///DESC Argument frame to pass to the next function invocation.
    ///FIELD lastErrorCode
        ///DESC Last error code caught by an error handler.
typedef struct localFrameHeader_t {
    allocPointer_t(runningApp_t) implementer;
    int32_t functionIndex;
    allocPointer_t(localFrame_t) previousLocalFrame;
    allocPointer_t(argFrame_t) nextArgFrame;
    int8_t lastErrorCode;
} localFrameHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
///STRUCT
    ///DESC Contains variables used by a single function invocation. This struct is stored in the data region of a heap allocation.
    ///FIELD data
        ///TYPE :Array of bytes
typedef struct localFrame_t {
    localFrameHeader_t header;
    int8_t data[0];
} localFrame_t;
#pragma pack(pop)

#pragma pack(push, 1)
///STRUCT
    ///DESC Contains variables passed from one function invocation to another. This struct is stored in the data region of a heap allocation.
    ///FIELD data
        ///TYPE :Array of bytes
typedef struct argFrame_t {
    int8_t data[0];
} argFrame_t;
#pragma pack(pop)

///FUNC
    ///RET :Type of `memberName` in `runningAppHeader_t`
    ///DESC Retrieves a member of the running application header in the given running application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `runningAppHeader_t`.
#define getRunningAppMember(runningApp, memberName) \
    readStructByPointer(runningApp.genericPointer, readAlloc, runningAppHeader_t, memberName)
///FUNC
    ///RET void
    ///DESC Modifies a member of the running application header in the given running application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `runningAppHeader_t`.
    ///ARG value
        ///TYPE :Type of `memberName` in `runningAppHeader_t`
        ///DESC Value to write.
#define setRunningAppMember(runningApp, memberName, value) \
    writeStructByPointer(runningApp.genericPointer, writeAlloc, runningAppHeader_t, memberName, value)

///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the address of the global frame data region in the given running application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
#define getGlobalFrameDataAddress(runningApp) \
    (getAllocDataAddress(runningApp.genericPointer) + sizeof(runningAppHeader_t))
///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the size of the global frame data region in the given running application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
#define getGlobalFrameSize(runningApp) \
    (getAllocSize(runningApp.genericPointer) - sizeof(runningAppHeader_t))

///FUNC
    ///RET type
    ///DESC Reads a value from the global frame data region in the given running application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
    ///ARG index
        ///TYPE heapMemoryOffset_t
        ///DESC Offset of first byte to read.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to read.
#define readGlobalFrame(runningApp, index, type) \
    readHeapMemory(getGlobalFrameDataAddress(runningApp) + index, type)
///FUNC
    ///RET void
    ///DESC Writes a value to the global frame data region in the given running application.
    ///ARG runningApp
        ///TYPE allocPointer_t(runningApp_t)
    ///ARG index
        ///TYPE heapMemoryOffset_t
        ///DESC Offset of first byte to write.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to write.
    ///ARG value
        ///TYPE type
        ///DESC Value to write.
#define writeGlobalFrame(runningApp, index, type, value) \
    writeHeapMemory(getGlobalFrameDataAddress(runningApp) + index, type, value)

///FUNC
    ///RET :Type of `memberName` in `localFrameHeader_t`
    ///DESC Retrieves a member of the local frame header in the given local frame.
    ///ARG localFrame
        ///TYPE allocPointer_t(localFrame_t)
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `localFrameHeader_t`.
#define getLocalFrameMember(localFrame, memberName) \
    readStructByPointer(localFrame.genericPointer, readAlloc, localFrameHeader_t, memberName)
///FUNC
    ///RET void
    ///DESC Modifies a member of the local frame header in the given local frame.
    ///ARG localFrame
        ///TYPE allocPointer_t(localFrame_t)
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `localFrameHeader_t`.
    ///ARG value
        ///TYPE :Type of `memberName` in `localFrameHeader_t`
        ///DESC Value to write.
#define setLocalFrameMember(localFrame, memberName, value) \
    writeStructByPointer(localFrame.genericPointer, writeAlloc, localFrameHeader_t, memberName, value)

///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the address of the data region in the given local frame.
    ///ARG localFrame
        ///TYPE allocPointer_t(localFrame_t)
#define getLocalFrameDataAddress(localFrame) \
    (getAllocDataAddress(localFrame.genericPointer) + sizeof(localFrameHeader_t))
///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the size of the data region in the given local frame.
    ///ARG localFrame
        ///TYPE allocPointer_t(localFrame_t)
#define getLocalFrameSize(localFrame) \
    (getAllocSize(localFrame.genericPointer) - sizeof(localFrameHeader_t))

///FUNC
    ///RET type
    ///DESC Reads a value from the data region in the given local frame.
    ///ARG localFrame
        ///TYPE allocPointer_t(localFrame_t)
    ///ARG index
        ///TYPE heapMemoryOffset_t
        ///DESC Offset of first byte to read.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to read.
#define readLocalFrame(localFrame, index, type) \
    readHeapMemory(getLocalFrameDataAddress(localFrame) + index, type)
///FUNC
    ///RET void
    ///DESC Writes a value to the data region in the given local frame.
    ///ARG localFrame
        ///TYPE allocPointer_t(localFrame_t)
    ///ARG index
        ///TYPE heapMemoryOffset_t
        ///DESC Offset of first byte to write.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to write.
    ///ARG value
        ///TYPE type
        ///DESC Value to write.
#define writeLocalFrame(localFrame, index, type, value) \
    writeHeapMemory(getLocalFrameDataAddress(localFrame) + index, type, value)

///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the address of the data region in the given argument frame.
    ///ARG argFrame
        ///TYPE allocPointer_t(argFrame_t)
#define getArgFrameDataAddress(argFrame) getAllocDataAddress(argFrame.genericPointer)
///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the size of the data region in the given argument frame.
    ///ARG argFrame
        ///TYPE allocPointer_t(argFrame_t)
#define getArgFrameSize(argFrame) getAllocSize(argFrame.genericPointer)

///FUNC
    ///RET type
    ///DESC Reads a value from the data region in the given argument frame.
    ///ARG argFrame
        ///TYPE allocPointer_t(argFrame_t)
    ///ARG index
        ///TYPE heapMemoryOffset_t
        ///DESC Offset of first byte to read.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to read.
#define readArgFrame(argFrame, index, type) \
    readAlloc(argFrame.genericPointer, index, type)
///FUNC
    ///RET type
    ///DESC Writes a value to the data region in the given argument frame.
    ///ARG argFrame
        ///TYPE allocPointer_t(argFrame_t)
    ///ARG index
        ///TYPE heapMemoryOffset_t
        ///DESC Offset of first byte to write.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to write.
    ///ARG value
        ///TYPE type
        ///DESC Value to write.
#define writeArgFrame(argFrame, index, type, value) \
    writeAlloc(argFrame.genericPointer, index, type, value)

///FUNC
    ///DESC Retrieves the argument frame which has been passed to the current function invocation.
    ///RET allocPointer_t(argFrame_t)
#define getPreviousArgFrame() ({ \
    allocPointer_t(localFrame_t) tempLocalFrame = getLocalFrameMember(currentLocalFrame, previousLocalFrame); \
    getLocalFrameMember(tempLocalFrame, nextArgFrame); \
})
///FUNC
    ///DESC Deletes any argument frame which has been created by the current function invocation.
    ///RET void
#define cleanUpNextArgFrame() cleanUpNextArgFrameHelper(currentLocalFrame)

///VAR
    ///DESC Stores the running appliction of the currently scheduled thread.
allocPointer_t(runningApp_t) currentThreadApp;
///VAR
    ///DESC Stores the active local frame of the currently scheduled function invocation.
allocPointer_t(localFrame_t) currentLocalFrame;
///VAR
    ///DESC Stores the running application which implements the currently scheduled function invocation.
allocPointer_t(runningApp_t) currentImplementer;
///VAR
    ///DESC Stores the file handle of the application which implements the currently scheduled function invocation.
allocPointer_t(fileHandle_t) currentImplementerFileHandle;

///FUNC
    ///DESC Retrieves an index in the function table of the given running application.
    ///ARG functionId
        ///DESC ID of function to find.
int32_t findFunctionById(allocPointer_t(runningApp_t) runningApp, int32_t functionId);
///FUNC
    ///DESC Retrieves the running application which implements the caller of the currently active function invocation.
allocPointer_t(runningApp_t) getCurrentCaller();
///FUNC
    ///DESC Creates an argument frame to be passed to the next function invocation.
    ///ARG size
        ///DESC Size of data region in the new argument frame.
allocPointer_t(argFrame_t) createNextArgFrame(heapMemoryOffset_t size);
void cleanUpNextArgFrameHelper(allocPointer_t(localFrame_t) localFrame);
///FUNC
    ///DESC Launches a running application from the given file handle.
void launchApp(allocPointer_t(fileHandle_t) fileHandle);
///FUNC
    ///DESC Invokes a function in the given thread.
    ///ARG threadApp
        ///DESC Running application of the thread to use.
    ///ARG implementer
        ///DESC Running application which implements the function.
    ///ARG functionIndex
        ///DESC Index of the function in `implementer`.
void callFunction(
    allocPointer_t(runningApp_t) threadApp,
    allocPointer_t(runningApp_t) implementer,
    int32_t functionIndex
);
///FUNC
    ///DESC Stops evaluation of the current function invocation, and returns control to the previous function invocation.
void returnFromFunction();
///FUNC
    ///DESC Enters a blocking loop to run all WheatSystem applications.
void runAppSystem();


