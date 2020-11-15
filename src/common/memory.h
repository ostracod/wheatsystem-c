
#define true 1
#define false 0

#define getStructMemberOffset(structDefinition, memberName) \
    (int32_t)((void *)&((structDefinition *)NULL)->memberName - NULL)
#define getStructMemberType(structDefinition, memberName) \
    typeof(((structDefinition *)NULL)->memberName)
#define readArrayConstantElement(name, index) \
    !!!readArrayElementByPointer name readArrayConstantValue index getArrayConstantElementType(name)


