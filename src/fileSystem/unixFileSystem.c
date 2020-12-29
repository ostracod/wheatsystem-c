
#include "./headers.h"

#define UNIX_FILE_HEADER_SIZE 1

int8_t *convertFileNameToUnixPath(int8_t *name) {
    int8_t *output;
    asprintf((char **)&output, "%s/%s", unixVolumePath, name);
    return output;
}

int8_t initializeFileSystem() {
    DIR *volumeDirectory = opendir((char *)unixVolumePath);
    if (!volumeDirectory) {
        printf("Could not find volume directory.\n");
        return false;
    }
    closedir(volumeDirectory);
    return true;
}

allocPointer_t(fileHandle_t) openFile(
    heapMemoryOffset_t nameAddress,
    heapMemoryOffset_t nameSize
) {
    
    // Copy name from heap memory to native memory.
    int8_t *nativeName = malloc(nameSize + 1);
    for (heapMemoryOffset_t index = 0; index < nameSize; index++) {
        nativeName[index] = readHeapMemory(nameAddress + index, int8_t);
    }
    nativeName[nameSize] = 0;
    
    // Return matching file handle if it already exists.
    genericAllocPointer_t nextPointer = getFirstAlloc();
    while (nextPointer != NULL_ALLOC_POINTER) {
        genericAllocPointer_t tempPointer = nextPointer;
        nextPointer = getAllocNext(tempPointer);
        if (!allocIsFileHandle(tempPointer)) {
            continue;
        }
        allocPointer_t(fileHandle_t) fileHandle = castGenericPointer(
            tempPointer,
            fileHandle_t
        );
        int8_t *tempNativeName = getFileHandleMember(fileHandle, name);
        if (strcmp((char *)tempNativeName, (char *)nativeName) != 0) {
            continue;
        }
        free(nativeName);
        int8_t tempDepth = getFileHandleMember(fileHandle, openDepth);
        setFileHandleMember(fileHandle, openDepth, tempDepth + 1);
        return fileHandle;
    }
    
    // Try to open native file.
    int8_t *unixPath = convertFileNameToUnixPath(nativeName);
    FILE *nativeFileHandle = fopen((char *)unixPath, "r");
    if (nativeFileHandle == NULL) {
        // File is missing.
        free(nativeName);
        free(unixPath);
        return nullAllocPointer(fileHandle_t);
    }
    
    // Read file content.
    int32_t contentSize = getNativeFileSize(nativeFileHandle) - UNIX_FILE_HEADER_SIZE;
    int8_t fileAttributes;
    fread(&fileAttributes, 1, 1, nativeFileHandle);
    int8_t *content = malloc(contentSize);
    fread(content, 1, contentSize, nativeFileHandle);
    fclose(nativeFileHandle);
    
    // Create file handle.
    allocPointer_t(fileHandle_t) output = castAllocPointer(
        createDynamicAlloc(
            sizeof(fileHandle_t),
            GUARDED_ALLOC_ATTR | SENTRY_ALLOC_ATTR,
            nullAllocPointer(fileHandle_t)
        ),
        fileHandle_t
    );
    setFileHandleMember(output, name, nativeName);
    setFileHandleMember(output, unixPath, unixPath);
    setFileHandleMember(
        output,
        hasAdminPerm,
        getHasAdminPermFromFileAttributes(fileAttributes)
    );
    setFileHandleMember(
        output,
        isGuarded,
        getIsGuardedFromFileAttributes(fileAttributes)
    );
    setFileHandleMember(
        output,
        type,
        getTypeFromFileAttributes(fileAttributes)
    );
    setFileHandleMember(output, contentSize, contentSize);
    setFileHandleMember(output, contentIsDirty, false);
    setFileHandleMember(output, content, content);
    setFileHandleRunningApp(output, nullAllocPointer(runningApp_t));
    setFileHandleInitErr(output, 0);
    setFileHandleMember(output, openDepth, 1);
    return output;
}

void closeFile(allocPointer_t(fileHandle_t) fileHandle) {
    int8_t *unixPath = getFileHandleMember(fileHandle, unixPath);
    int8_t *content = getFileHandleMember(fileHandle, content);
    if (getFileHandleMember(fileHandle, contentIsDirty)) {
        FILE *nativeFileHandle = fopen((char *)unixPath, "w");
        int8_t fileAttributes = getFileHandleMember(fileHandle, type);
        if (getFileHandleMember(fileHandle, hasAdminPerm)) {
            fileAttributes |= 0x08;
        }
        if (getFileHandleMember(fileHandle, isGuarded)) {
            fileAttributes |= 0x04;
        }
        int32_t contentSize = getFileHandleMember(fileHandle, contentSize);
        fwrite(&fileAttributes, 1, 1, nativeFileHandle);
        fwrite(content, 1, contentSize, nativeFileHandle);
        fclose(nativeFileHandle);
    }
    int8_t openDepth = getFileHandleMember(fileHandle, openDepth);
    if (openDepth > 1) {
        setFileHandleMember(fileHandle, openDepth, openDepth - 1);
        return;
    }
    int8_t *nativeName = getFileHandleMember(fileHandle, name);
    free(nativeName);
    free(unixPath);
    free(content);
    deleteAlloc(fileHandle.genericPointer);
}


