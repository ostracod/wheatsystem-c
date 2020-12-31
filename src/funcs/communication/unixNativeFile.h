
///DESC This file provides platform-specific functions for interacting with the native Unix file system. A platform requires these definitions when using `funcs/fileSystem/unixFileSystem` or `funcs/fileSystem/unixStorageSpace`.

#include <stdio.h>

///FUNC
    ///DESC Sets the value of `unixVolumePath`, and removes any trailing slash.
    ///ARG path
        ///DESC Native Unix path of a directory.
void initializeUnixVolumePath(int8_t *path);
///FUNC
    ///DESC Retrieves the number of bytes in the given file.
    ///ARG fileHandle
        ///DESC Native Unix file handle.
int32_t getNativeFileSize(FILE *fileHandle);


