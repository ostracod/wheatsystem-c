
///DESC This file provides common definitions pertaining to errors. Every platform shares the same implementation of these definitions.

///CONST
    ///TYPE int8_t
#define GENERIC_ERR_CODE 0x01
///CONST
    ///TYPE int8_t
#define NO_IMPL_ERR_CODE 0x02
///CONST
    ///TYPE int8_t
#define TYPE_ERR_CODE 0x03
///CONST
    ///TYPE int8_t
#define NUM_RANGE_ERR_CODE 0x04
///CONST
    ///TYPE int8_t
#define INDEX_ERR_CODE 0x05
///CONST
    ///TYPE int8_t
#define PTR_ERR_CODE 0x06
///CONST
    ///TYPE int8_t
#define NULL_ERR_CODE 0x07
///CONST
    ///TYPE int8_t
#define DATA_ERR_CODE 0x08
///CONST
    ///TYPE int8_t
#define ARG_FRAME_ERR_CODE 0x09
///CONST
    ///TYPE int8_t
#define MISSING_ERR_CODE 0x0A
///CONST
    ///TYPE int8_t
#define STATE_ERR_CODE 0x0B
///CONST
    ///TYPE int8_t
#define PERM_ERR_CODE 0x0C
///CONST
    ///TYPE int8_t
#define CAPACITY_ERR_CODE 0x0D
///CONST
    ///TYPE int8_t
#define THROTTLE_ERR_CODE 0x0E

///VAR
    ///DESC Stores the last thrown error code. The value of this variable must be checked after invoking certain functions.
int8_t unhandledErrorCode;


