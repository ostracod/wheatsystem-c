
///DESC This file provides an implementation of the functions decribed by `abstract/memory/arrayConstant`.

#define readArrayConstantValue(name, index, type) *(type *)((int8_t *)name + index)


