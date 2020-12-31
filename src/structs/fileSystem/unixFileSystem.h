
///DESC This file provides an implementation of the data structures decribed by `abstract/fileSystem/fileSystem`.

#pragma pack(push, 1)
typedef struct fileHandle_t {
    int8_t *name;
    int8_t *unixPath;
    int8_t hasAdminPerm;
    int8_t isGuarded;
    int8_t type;
    int32_t contentSize;
    int8_t contentIsDirty;
    int8_t *content;
    allocPointer_t(runningApp_t) runningApp;
    int8_t initErr;
    int8_t openDepth;
} fileHandle_t;
#pragma pack(pop)


