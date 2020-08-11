
#define true 1
#define false 0

#define getStructMemberOffset(structDefinition, memberName) \
    (int32_t)((void *)&((structDefinition *)NULL)->memberName - NULL)
#define getStructMemberType(structDefinition, memberName) \
    typeof(((structDefinition *)NULL)->memberName)

#define readStructMember(pointer, readFunction, structDefinition, memberName) \
    readFunction(pointer, getStructMemberOffset(structDefinition, memberName), getStructMemberType(structDefinition, memberName))
#define writeStructMember(pointer, writeFunction, structDefinition, memberName, value) \
    writeFunction(pointer, getStructMemberOffset(structDefinition, memberName), getStructMemberType(structDefinition, memberName), value)

// TODO: Eliminate the need for this macro.
// The C preprocessor really hates recursive macros...
#define readStructMember2(pointer, readFunction, structDefinition, memberName) \
    readFunction(pointer, getStructMemberOffset(structDefinition, memberName), getStructMemberType(structDefinition, memberName))


