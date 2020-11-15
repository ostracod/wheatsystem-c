
#include "./headers.h"

void printDebugString(int8_t *text) {
    while (true) {
        int8_t tempCharacter = *text;
        if (tempCharacter == 0) {
            break;
        }
        printDebugCharacter(tempCharacter);
        text += 1;
    }
}

void printDebugStringConstant(arrayConstant_t(int8_t) stringConstant) {
    int32_t index = 0;
    while (true) {
        int8_t tempCharacter = readArrayConstantElement(stringConstant, index);
        if (tempCharacter == 0) {
            break;
        }
        printDebugCharacter(tempCharacter);
        index += 1;
    }
}

void printDebugNumber(int32_t number) {
    int8_t tempText[15];
    convertNumberToText(tempText, number);
    printDebugString(tempText);
}


