
#pragma pack(push, 1)
typedef struct termAppGlobalFrame {
    int32_t width;
    int32_t height;
    allocPointer_t observer;
    int32_t termInputIndex;
} termAppGlobalFrame_t;
#pragma pack(pop)

#define TERM_APP_GLOBAL_FRAME_SIZE sizeof(termAppGlobalFrame_t)

void initializeTermApp();
void setTermObserver();
void getTermSize();
void writeTermText();


