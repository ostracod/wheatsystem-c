
///DESC This file provides an implementation of the functions described by `abstract/systemApp/termApp`. This implementation uses an LCD connected to the SPI bus of the AVR chip. This implemenation of `abstract/systemApp/termApp` requires the definitions provided by `functs/communication/avrSpi`.

void releaseLcdSpiDevice();
void initializeTermApp();
void setTermObserver();
void getTermSize();
void writeTermText();


