
#define FILE_HANDLE_NAME_OFFSET 0
#define FILE_HANDLE_CONTENT_OFFSET (FILE_HANDLE_NAME_OFFSET + sizeof(int8_t *))
#define FILE_HANDLE_APP_OFFSET (FILE_HANDLE_CONTENT_OFFSET + sizeof(int8_t *))
#define FILE_HANDLE_INIT_ERROR_OFFSET (FILE_HANDLE_APP_OFFSET + sizeof(allocPointer_t))
#define FILE_HANDLE_OPEN_DEPTH_OFFSET (FILE_HANDLE_INIT_ERROR_OFFSET + 1)
#define FILE_HANDLE_SIZE (FILE_HANDLE_OPEN_DEPTH_OFFSET + 1)

#define getFileHandleApp(pointer) \
    readDynamicAlloc(pointer, FILE_HANDLE_APP_OFFSET, allocPointer_t)
#define getFileHandleInitErr(pointer) \
    readDynamicAlloc(pointer, FILE_HANDLE_INIT_ERROR_OFFSET, int8_t)

#define setFileHandleApp(pointer, app) \
    writeDynamicAlloc(pointer, FILE_HANDLE_APP_OFFSET, allocPointer_t, app)
#define setFileHandleInitErr(pointer, initErr) \
    writeDynamicAlloc(pointer, FILE_HANDLE_INIT_ERROR_OFFSET, int8_t, initErr)


