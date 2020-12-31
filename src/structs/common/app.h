
///DESC This file provides common data structures for applications. Every platform shares the same implementation of these definitions.

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


