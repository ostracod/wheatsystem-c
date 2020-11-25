
#include "./headers.h"

declareArrayConstantWithValue(termAppFunctionArray, systemAppFunction_t, {
    (systemAppFunction_t){INIT_FUNC_ID, 0, 0, initializeTermApp},
    (systemAppFunction_t){LISTEN_TERM_FUNC_ID, 0, 0, setTermObserver},
    (systemAppFunction_t){TERM_SIZE_FUNC_ID, 8, 0, getTermSize},
    (systemAppFunction_t){WRT_TERM_FUNC_ID, 12, 0, writeTermText}
});


