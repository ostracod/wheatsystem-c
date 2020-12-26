
///DESC This file provides common definitions for general-purpose memory operations. Every platform shares the same implementation of these definitions.

///CONST
    ///TYPE int8_t
#define true 1
///CONST
    ///TYPE int8_t
#define false 0

///FUNC
    ///RET int32_t
    ///DESC Retrieves the location of `memberName` in `structDefinition`, measured as the number of bytes from the beginning of the struct.
    ///ARG structDefinition
        ///TYPE :Struct type
    ///ARG memberName
        ///TYPE :Identifier
#define getStructMemberOffset(structDefinition, memberName) \
    (int32_t)((void *)&((structDefinition *)NULL)->memberName - NULL)
///FUNC
    ///RET :Type
    ///DESC Retrieves the type of `memberName` in `structDefinition`.
    ///ARG structDefinition
        ///TYPE :Struct type
    ///ARG memberName
        ///TYPE :Identifier
#define getStructMemberType(structDefinition, memberName) \
    typeof(((structDefinition *)NULL)->memberName)
///FUNC
    ///RET `type`
    ///DESC Reads an element within the given array constant.
    ///ARG arrayConstant
        ///TYPE arrayConstant_t(type)
    ///ARG index
        ///TYPE int32_t
        ///DESC Element index, measured as the number of elements from the beginning of the array.
#define readArrayConstantElement(arrayConstant, index) \
    readArrayElementByPointer(arrayConstant, readArrayConstantValue, index, getArrayConstantElementType(arrayConstant))


