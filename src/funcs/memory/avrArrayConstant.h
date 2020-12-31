
///DESC This file provides an implementation of the functions decribed by `abstract/memory/arrayConstant`.

#define readArrayConstantValue(name, index, type) ({int8_t result[sizeof(type)]; memcpy_P(&result, (int8_t *)name + index, sizeof(type)); *(type *)result;})


