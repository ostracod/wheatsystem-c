
#include "./headers.h"

#define UNIX_FILE_HEADER_SIZE 1

int8_t *convertFileNameToUnixPath(int8_t *name) {
    int8_t *output;
    asprintf((char **)&output, "%s/%s", unixVolumePath, name);
    return output;
}

allocPointer_t openFile(heapMemoryOffset_t nameAddress, heapMemoryOffset_t nameSize) {
    
    // Copy name from heap memory to native memory.
    int8_t *nativeName = malloc(nameSize + 1);
    for (heapMemoryOffset_t index = 0; index < nameSize; index++) {
        nativeName[index] = readHeapMemory(nameAddress + index, int8_t);
    }
    nativeName[nameSize] = 0;
    
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
    
    // Try to open native file.
    int8_t *unixPath = convertFileNameToUnixPath(nativeName);
    FILE *nativeFileHandle = fopen((char *)unixPath, "r");
    if (nativeFileHandle == NULL) {
        // File is missing.
        free(nativeName);
        free(unixPath);
        return NULL_ALLOC_POINTER;
    }
    
    // Read file content.
    int8_t fileMetadata;
    fread(&fileMetadata, 1, 1, nativeFileHandle);
    fseek(nativeFileHandle, 0, SEEK_END);
    int32_t contentSize = (int32_t)(ftell(nativeFileHandle) - UNIX_FILE_HEADER_SIZE);
    int8_t *content = malloc(contentSize);
    fseek(nativeFileHandle, UNIX_FILE_HEADER_SIZE, SEEK_SET);
    fread(content, 1, contentSize, nativeFileHandle);
    fclose(nativeFileHandle);
    
    // Create file handle.
    allocPointer_t output = createDynamicAlloc(
        sizeof(fileHandle_t),
        true,
        NULL_ALLOC_POINTER
    );
    setFileHandleMember(output, name, nativeName);
    setFileHandleMember(output, unixPath, unixPath);
    setFileHandleMember(output, hasAdminPerm, (fileMetadata & 0x08) > 0);
    setFileHandleMember(output, isGuarded, (fileMetadata & 0x04) > 0);
    setFileHandleMember(output, type, fileMetadata & 0x03);
    setFileHandleMember(output, contentSize, contentSize);
    setFileHandleMember(output, contentIsDirty, false);
    setFileHandleMember(output, content, content);
    setFileHandleMember(output, openDepth, 1);
    setFileHandleRunningApp(output, NULL_ALLOC_POINTER);
    setFileHandleInitErr(output, 0);
    return output;
}

void closeFile(allocPointer_t fileHandle) {
    int8_t openDepth = getFileHandleMember(fileHandle, openDepth);
    if (openDepth > 1) {
        setFileHandleMember(fileHandle, openDepth, openDepth - 1);
        return;
    }
    int8_t *nativeName = getFileHandleMember(fileHandle, name);
    int8_t *unixPath = getFileHandleMember(fileHandle, unixPath);
    int8_t *content = getFileHandleMember(fileHandle, content);
    if (getFileHandleMember(fileHandle, contentIsDirty)) {
        FILE *nativeFileHandle = fopen((char *)unixPath, "w");
        int8_t fileMetadata = 0x20 | getFileHandleMember(fileHandle, type);
        if (getFileHandleMember(fileHandle, hasAdminPerm)) {
            fileMetadata |= 0x08;
        }
        if (getFileHandleMember(fileHandle, isGuarded)) {
            fileMetadata |= 0x04;
        }
        int32_t contentSize = getFileHandleMember(fileHandle, contentSize);
        fwrite(&fileMetadata, 1, 1, nativeFileHandle);
        fwrite(content, 1, contentSize, nativeFileHandle);
        fclose(nativeFileHandle);
    }
    free(nativeName);
    free(unixPath);
    free(content);
    deleteAlloc(fileHandle);
}


