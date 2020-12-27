
#include "./headers.h"
#include <curses.h>

WINDOW *window = NULL;

void handleResize() {
    int32_t tempWidth;
    int32_t tempHeight;
    getmaxyx(window, tempHeight, tempWidth);
    writeTermAppGlobalVariable(width, tempWidth);
    writeTermAppGlobalVariable(height, tempHeight);
}

void initializeTermApp() {
    if (window == NULL) {
        window = initscr();
        noecho();
        curs_set(0);
        keypad(window, true);
        ESCDELAY = 50;
        timeout(0);
        handleResize();
    }
    allocPointer_t(runningApp_t) tempObserver = readTermAppGlobalVariable(observer);
    if (pointerIsNull(tempObserver)) {
        return;
    }
    // TODO: Check whether observer is still running.
    
    int32_t tempKey = getch();
    if (tempKey < 32 || tempKey > 127) {
        if (tempKey == KEY_LEFT) {
            tempKey = -1;
        } else if (tempKey == KEY_RIGHT) {
            tempKey = -2;
        } else if (tempKey == KEY_UP) {
            tempKey = -3;
        } else if (tempKey == KEY_DOWN) {
            tempKey = -4;
        } else if (tempKey == 263) {
            tempKey = 127;
        } else if (tempKey != 10 && tempKey != 27) {
            return;
        }
    }
    allocPointer_t(argFrame_t) nextArgFrame = createNextArgFrame(1);
    writeArgFrame(nextArgFrame, 0, int8_t, (int8_t)tempKey);
    int32_t termInputIndex = readTermAppGlobalVariable(termInputIndex);
    callFunction(currentThreadApp, tempObserver, termInputIndex);
}

void setTermObserver() {
    allocPointer_t(runningApp_t) tempCaller = getCurrentCaller();
    int32_t termInputIndex = findFunctionById(tempCaller, TERM_INPUT_FUNC_ID);
    if (termInputIndex < 0) {
        unhandledErrorCode = MISSING_ERR_CODE;
    } else {
        writeTermAppGlobalVariable(observer, tempCaller);
        writeTermAppGlobalVariable(termInputIndex, termInputIndex);
    }
    returnFromFunction();
}

void getTermSize() {
    int32_t tempWidth = readTermAppGlobalVariable(width);
    int32_t tempHeight = readTermAppGlobalVariable(height);
    allocPointer_t(argFrame_t) previousArgFrame = getPreviousArgFrame();
    writeArgFrame(previousArgFrame, 0, int32_t, tempWidth);
    writeArgFrame(previousArgFrame, 4, int32_t, tempHeight);
    returnFromFunction();
}

void writeTermText() {
    allocPointer_t(argFrame_t) previousArgFrame = getPreviousArgFrame();
    int32_t posX = readArgFrame(previousArgFrame, 0, int32_t);
    int32_t posY = readArgFrame(previousArgFrame, 4, int32_t);
    allocPointer_t(dynamicAlloc_t) textAlloc = castGenericPointer(
        readArgFrame(previousArgFrame, 8, int32_t),
        dynamicAlloc_t
    );
    heapMemoryOffset_t textSize = getDynamicAllocSize(textAlloc);
    wmove(window, posX, posY);
    for (heapMemoryOffset_t index = 0; index < textSize; index++) {
        int8_t tempCharacter = readDynamicAlloc(textAlloc, index, int8_t);
        waddch(window, (char)tempCharacter);
    }
    refresh();
    returnFromFunction();
}


