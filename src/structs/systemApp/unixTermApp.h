
///DESC This file provides an implementation of the data structures described by `abstract/systemApp/termApp`.

#pragma pack(push, 1)
typedef struct termAppGlobalFrame_t {
    int32_t width;
    int32_t height;
    allocPointer_t(runningApp_t) observer;
    int32_t termInputIndex;
} termAppGlobalFrame_t;
#pragma pack(pop)


