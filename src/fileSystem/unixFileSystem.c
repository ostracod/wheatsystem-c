
#include "../../intermediate/headers.h"

allocPointer_t openFile(heapMemoryOffset_t nameAddress, heapMemoryOffset_t nameSize) {
    // Copy name from heap memory to native memory.
    int8_t *nativeName = malloc(nameSize + 1);
    for (heapMemoryOffset_t index = 0; index < nameSize; index++) {
        nativeName[index] = readHeapMemory(nameAddress + index, int8_t);
    }
    nativeName[nameSize - 1] = 0;
    // Return matching file handle if it already exists.
    allocPointer_t nextPointer = getFirstAlloc();
    while (nextPointer != NULL_ALLOC_POINTER) {
        allocPointer_t tempPointer = nextPointer;
        nextPointer = getAllocNext(tempPointer);
        if (!allocIsFileHandle(tempPointer)) {
            continue;
        }
        int8_t *tempNativeName = getFileHandleMember(tempPointer, name);
        if (strcmp((char *)tempNativeName, (char *)nativeName) != 0) {
            continue;
        }
        free(nativeName);
        int8_t tempDepth = getFileHandleMember(tempPointer, openDepth);
        setFileHandleMember(tempPointer, openDepth, tempDepth + 1);
        return tempPointer;
    }
    // TODO: Verify that the file actually exists.
    allocPointer_t output = createDynamicAlloc(
        sizeof(fileHandle_t),
        true,
        NULL_ALLOC_POINTER
    );
    setFileHandleMember(output, name, nativeName);
    // TODO: Read file content.
    setFileHandleMember(output, content, NULL);
    setFileHandleMember(output, openDepth, 1);
    setFileHandleApp(output, NULL_ALLOC_POINTER);
    setFileHandleInitErr(output, 0);
    return output;
}

void closeFile(allocPointer_t fileHandle) {
    // TODO: Implement.
    
}


