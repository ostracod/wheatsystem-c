
///DESC This file provides platform-specific definitions for interacting with the native Unix file system. A platform only requires these definitions when using `fileSystem/unixFileSystem`.

///VAR
    ///DESC The Unix path of the directory to use for WheatSystem files. Use `initializeUnixVolumePath` to set the value of this variable.
int8_t *unixVolumePath;

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


