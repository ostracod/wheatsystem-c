
#pragma pack(push, 1)
typedef struct termAppGlobalFrame {
    allocPointer_t observer;
} termAppGlobalFrame_t;
#pragma pack(pop)

#define TERM_APP_GLOBAL_FRAME_SIZE sizeof(termAppGlobalFrame_t)

declareArrayConstantWithLength(termAppFunctionArray, systemAppFunction_t, 4);

void releaseLcdSpiDevice();


