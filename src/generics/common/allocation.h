
///DESC This file provides common parameterizable types for heap allocations. Every platform shares the same implementation of these definitions.

///FUNC
    ///RET type
    ///DESC Retrieves the type of data referenced by the given pointer.
    ///ARG pointer
        ///TYPE allocPointer_t(type)
#define getAllocPointerType(pointer) typeof(*(((typeof(pointer) *)NULL)->typeCarrier[0]))


