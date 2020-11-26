
#define readTermAppGlobalVariable(memberName) \
    readSystemAppGlobalVariable(termAppGlobalFrame_t, memberName)
#define writeTermAppGlobalVariable(memberName, value) \
    writeSystemAppGlobalVariable(termAppGlobalFrame_t, memberName, value)

declareArrayConstantWithLength(termAppFunctionArray, systemAppFunction_t, 4);


