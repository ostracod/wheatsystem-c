
#include "./headers.h"

declareArrayConstantWithValue(systemAppArray, systemApp_t, {
    createSystemApp(TERM_APP_ID, TERM_APP_GLOBAL_FRAME_SIZE, termAppFunctionArray)
});


