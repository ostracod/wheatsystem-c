
///DESC This file provides an implementation of the definitions described by `systemApp/termApp`. This implementation uses an LCD connected to the SPI bus of the AVR chip.

#pragma pack(push, 1)
typedef struct termAppGlobalFrame {
    allocPointer_t observer;
} termAppGlobalFrame_t;
#pragma pack(pop)

void releaseLcdSpiDevice();
void initializeTermApp();
void setTermObserver();
void getTermSize();
void writeTermText();


