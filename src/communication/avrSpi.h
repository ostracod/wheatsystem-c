
#define EEPROM_SPI_DEVICE_ID 1
#define LCD_SPI_DEVICE_ID 2

void initializeSpi();
void acquireSpiDevice(int8_t id);
int8_t receiveSpiInt8();
void sendSpiInt8(int8_t value);


