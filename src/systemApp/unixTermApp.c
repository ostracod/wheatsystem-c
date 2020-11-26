
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
    if (window != NULL) {
        return;
    }
    window = initscr();
    noecho();
    curs_set(0);
    keypad(window, true);
    ESCDELAY = 50;
    handleResize();
    returnFromFunction();
}

void setTermObserver() {
    // TODO: Implement.
    
}

void getTermSize() {
    int32_t tempWidth = readTermAppGlobalVariable(width);
    int32_t tempHeight = readTermAppGlobalVariable(height);
    allocPointer_t previousArgFrame = getPreviousArgFrame();
    writeAlloc(previousArgFrame, 0, int32_t, tempWidth);
    writeAlloc(previousArgFrame, 4, int32_t, tempHeight);
    returnFromFunction();
}

void writeTermText() {
    allocPointer_t previousArgFrame = getPreviousArgFrame();
    int32_t posX = readAlloc(previousArgFrame, 0, int32_t);
    int32_t posY = readAlloc(previousArgFrame, 4, int32_t);
    allocPointer_t textAlloc = (allocPointer_t)readAlloc(previousArgFrame, 8, int32_t);
    heapMemoryOffset_t textSize = getDynamicAllocSize(textAlloc);
    wmove(window, posX, posY);
    for (heapMemoryOffset_t index = 0; index < textSize; index++) {
        int8_t tempCharacter = readDynamicAlloc(textAlloc, index, int8_t);
        waddch(window, (char)tempCharacter);
    }
    refresh();
    returnFromFunction();
}


