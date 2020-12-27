
///DESC This file provides an implementation of the definitions described by `systemApp/termApp`. This implementation uses the Curses library to display text on the Unix terminal.

#pragma pack(push, 1)
typedef struct termAppGlobalFrame_t {
    int32_t width;
    int32_t height;
    allocPointer_t(runningApp_t) observer;
    int32_t termInputIndex;
} termAppGlobalFrame_t;
#pragma pack(pop)

void initializeTermApp();
void setTermObserver();
void getTermSize();
void writeTermText();


