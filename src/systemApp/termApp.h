
///DESC This file describes platform-specific definitions for terminal driver system apps. A platform only needs to implement these definitions when using certain implementations of `systemApp/systemApps`.

///TYPE termAppGlobalFrame_t
    ///DESC Struct to store global variables of a terminal driver.

///FUNC initializeTermApp
    ///RET void
    ///DESC Implements the `init` WheatSystem function.

///FUNC setTermObserver
    ///RET void
    ///DESC Implements the `listenTerm` WheatSystem function.

///FUNC getTermSize
    ///RET void
    ///DESC Implements the `termSize` WheatSystem function.

///FUNC writeTermText
    ///RET void
    ///DESC Implements the `wrtTerm` WheatSystem function.

///CONST
    ///DESC Global frame size of a terminal driver.
#define TERM_APP_GLOBAL_FRAME_SIZE sizeof(termAppGlobalFrame_t)


///FUNC
    ///RET :Type of `memberName` in `termAppGlobalFrame_t`
    ///DESC Retrieves a global variable of a term driver.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Global variable to read.
#define readTermAppGlobalVariable(memberName) \
    readSystemAppGlobalVariable(termAppGlobalFrame_t, memberName)
///FUNC
    ///RET void
    ///DESC Modifies a global variable of a term driver.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Global variable to modify.
    ///ARG value
        ///TYPE :Type of `memberName` in `termAppGlobalFrame_t`
        ///DESC Value to write.
#define writeTermAppGlobalVariable(memberName, value) \
    writeSystemAppGlobalVariable(termAppGlobalFrame_t, memberName, value)

///CONST termAppFunctionArray
    ///TYPE arrayConstant_t(systemAppFunction_t)
    ///DESC List of functions which the term driver implements.
declareArrayConstantWithLength(termAppFunctionArray, systemAppFunction_t, 4);


