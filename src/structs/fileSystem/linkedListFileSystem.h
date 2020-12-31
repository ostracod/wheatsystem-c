
///DESC This file provides an implementation of the data structures decribed by `abstract/fileSystem/fileSystem`.

#pragma pack(push, 1)
typedef struct fileHeader_t {
    uint8_t attributes;
    uint8_t nameSize;
    int32_t contentSize;
    int32_t next;
} fileHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fileHandle_t {
    int32_t address;
    uint8_t attributes;
    uint8_t nameSize;
    int32_t contentSize;
    allocPointer_t(runningApp_t) runningApp;
    int8_t initErr;
    int8_t openDepth;
} fileHandle_t;
#pragma pack(pop)


