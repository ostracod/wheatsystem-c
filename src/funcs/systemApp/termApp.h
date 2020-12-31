
///DESC This file provides platform-specific functions for terminal driver system apps.

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


