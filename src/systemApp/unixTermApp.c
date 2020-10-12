
#include "./headers.h"

void initializeTermApp() {
    // TODO: Implement.
    printf("Wow, this is the term init function!\n");
    returnFromFunction();
}

void setTermObserver() {
    // TODO: Implement.
    
}

void getTermSize() {
    // TODO: Implement.
    
}

void writeTermText() {
    // TODO: Implement.
    
}

declareArrayConstantWithValue(termAppFunctionArray, systemAppFunction_t, {
    (systemAppFunction_t){INIT_FUNC_ID, 0, initializeTermApp},
    (systemAppFunction_t){LISTEN_TERM_FUNC_ID, 0, setTermObserver},
    (systemAppFunction_t){TERM_SIZE_FUNC_ID, 0, getTermSize},
    (systemAppFunction_t){WRT_TERM_FUNC_ID, 0, writeTermText}
});


