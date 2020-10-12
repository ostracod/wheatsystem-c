
#define TERM_APP_ID 1
#define SERIAL_APP_ID 2
#define GPIO_APP_ID 3

#pragma pack(push, 1)
typedef struct systemAppFunction {
    int8_t id;
    int8_t localFrameSize;
    void (*threadAction)();
} systemAppFunction_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct systemApp {
    int8_t id;
    int8_t globalFrameSize;
    arrayConstant_t(systemAppFunction_t) functionList;
    int8_t functionAmount;
} systemApp_t;
#pragma pack(pop)

#define createSystemApp(appId, globalFrameSize, systemAppFunctionArray) (systemApp_t){appId, globalFrameSize, systemAppFunctionArray, getArrayConstantLength(systemAppFunctionArray)}


