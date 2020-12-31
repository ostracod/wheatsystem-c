
///DESC This file provides common data structures for system applications. Every platform shares the same implementation of these definitions.

///FUNC_TYPE systemAppThreadAction_t
    ///DESC Schedules work for a particular function of a system application.
    ///RET void
typedef void (*systemAppThreadAction_t)();

#pragma pack(push, 1)
///STRUCT
    ///DESC Defines a function available in a system application.
    ///FIELD id
        ///DESC ID of the function.
    ///FIELD threadAction
        ///DESC Task to perform whenever the function is scheduled.
typedef struct systemAppFunction_t {
    int8_t id;
    int8_t argFrameSize;
    int8_t localFrameSize;
    systemAppThreadAction_t threadAction;
} systemAppFunction_t;
#pragma pack(pop)

#pragma pack(push, 1)
///STRUCT
    ///DESC Defines a system application available on the current platform.
    ///FIELD functionAmount
        ///DESC Number of functions in `functionList`.
typedef struct systemApp_t {
    int8_t globalFrameSize;
    arrayConstant_t(systemAppFunction_t) functionList;
    int8_t functionAmount;
} systemApp_t;
#pragma pack(pop)

#pragma pack(push, 1)
///STRUCT
    ///DESC Stored at the beginning of the global frame of a system application.
    ///FIELD id
        ///DESC ID of the system application.
typedef struct systemGlobalFrameHeader_t {
    int8_t id;
} systemGlobalFrameHeader_t;
#pragma pack(pop)

///STRUCT systemGlobalFrame_t
    ///DESC Contents of the global frame of a system application. This struct is stored in the global frame of a `runningApp_t`.
    ///FIELD header
        ///TYPE systemGlobalFrameHeader_t
    ///FIELD data
        ///TYPE :Array of bytes


