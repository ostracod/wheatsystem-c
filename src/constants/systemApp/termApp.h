
///DESC This file provides platform-specific constants for terminal driver system apps.

///CONST
    ///TYPE int8_t
    ///DESC Global frame size of a terminal driver.
#define TERM_APP_GLOBAL_FRAME_SIZE sizeof(termAppGlobalFrame_t)

///CONST termAppFunctionArray
    ///TYPE arrayConstant_t(systemAppFunction_t)
    ///DESC List of functions which the term driver implements.
declareArrayConstantWithLength(termAppFunctionArray, systemAppFunction_t, 4);


