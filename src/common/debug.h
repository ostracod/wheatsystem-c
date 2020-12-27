
///FUNC
    ///RET void
    ///DESC Prints a newline character for debugging purposes.
#define printDebugNewline() printDebugCharacter('\n')

///FUNC
    ///DESC Prints a string from memory for debugging purposes.
    ///ARG text
        ///DESC Null-terminated string.
void printDebugString(int8_t *text);
///FUNC
    ///DESC Prints a string from the given array constant for debugging purposes.
    ///ARG stringConstant
        ///DESC Null-terminated string constant.
void printDebugStringConstant(arrayConstant_t(int8_t) stringConstant);
///FUNC
    ///DESC Prints the given integer in decimal for debugging purposes.
void printDebugNumber(int32_t number);


