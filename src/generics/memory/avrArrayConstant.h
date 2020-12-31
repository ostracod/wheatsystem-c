
///DESC This file provides an implementation of the parameterizable types described by `abstract/memory/arrayConstant`. This implementation uses the progmem space of the AVR chip to store array constants.

#include <avr/pgmspace.h>

#define arrayConstant_t(type) const type *
#define declareArrayConstantWithValue(name, type, ...) const type name[] PROGMEM = __VA_ARGS__
#define declareArrayConstantWithLength(name, type, size) const type name[size]
#define getArrayConstantLength(name) (int32_t)(sizeof(name) / sizeof(*name))
#define getArrayConstantElementType(name) typeof(*name)


