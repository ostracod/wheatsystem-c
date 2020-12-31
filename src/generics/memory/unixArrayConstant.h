
///DESC This file provides an implementation of the parameterizable types described by `abstract/memory/arrayConstant`. This implementation uses native memory to store array constants.

#define arrayConstant_t(type) type *
#define declareArrayConstantWithValue(name, type, ...) type name[] = __VA_ARGS__
#define declareArrayConstantWithLength(name, type, size) type name[size]
#define getArrayConstantLength(name) (int32_t)(sizeof(name) / sizeof(*name))
#define getArrayConstantElementType(name) typeof(*name)


