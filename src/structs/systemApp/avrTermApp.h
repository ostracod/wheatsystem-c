
///DESC This file provides an implementation of the data structures described by `abstract/systemApp/termApp`.

#pragma pack(push, 1)
typedef struct termAppGlobalFrame_t {
    allocPointer_t(runningApp_t) observer;
} termAppGlobalFrame_t;
#pragma pack(pop)


