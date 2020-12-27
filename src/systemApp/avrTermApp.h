
///DESC This file provides an implementation of the definitions described by `systemApp/termApp`. This implementation uses an LCD connected to the SPI bus of the AVR chip. This implemenation of `systemApp/termApp` requires the definitions provided by `communication/avrSpi`.

#pragma pack(push, 1)
typedef struct termAppGlobalFrame_t {
    allocPointer_t(runningApp_t) observer;
} termAppGlobalFrame_t;
#pragma pack(pop)

void releaseLcdSpiDevice();
void initializeTermApp();
void setTermObserver();
void getTermSize();
void writeTermText();


