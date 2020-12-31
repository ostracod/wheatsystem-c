
///DESC This file provides common global variables for running applications. Every platform shares the same implementation of these definitions.

///VAR
    ///DESC Stores the running appliction of the currently scheduled thread.
allocPointer_t(runningApp_t) currentThreadApp;
///VAR
    ///DESC Stores the active local frame of the currently scheduled function invocation.
allocPointer_t(localFrame_t) currentLocalFrame;
///VAR
    ///DESC Stores the running application which implements the currently scheduled function invocation.
allocPointer_t(runningApp_t) currentImplementer;
///VAR
    ///DESC Stores the file handle of the application which implements the currently scheduled function invocation.
allocPointer_t(fileHandle_t) currentImplementerFileHandle;


