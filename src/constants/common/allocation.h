
///DESC This file provides common constants for heap allocations. Every platform shares the same implementation of these definitions.

///CONST
    ///TYPE genericAllocPointer_t
#define NULL_ALLOC_POINTER 0

///CONST
    ///TYPE int8_t
#define RUNNING_APP_ALLOC_TYPE 0
///CONST
    ///TYPE int8_t
#define LOCAL_FRAME_ALLOC_TYPE 1
///CONST
    ///TYPE int8_t
#define ARG_FRAME_ALLOC_TYPE 2
///CONST
    ///TYPE int8_t
#define DYNAMIC_ALLOC_TYPE 3

///CONST
    ///TYPE int8_t
#define GUARDED_ALLOC_ATTR 1
///CONST
    ///TYPE int8_t
#define SENTRY_ALLOC_ATTR 2


