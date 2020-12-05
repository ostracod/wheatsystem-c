
const fs = require("fs");
const pathUtils = require("path");

const sourcePath = pathUtils.join(__dirname, "src");
const commentPrefix = "///";
const commentIndentation = "    ";
const openEnclosureCharacterSet = "([{";
const closeEnclosureCharacterSet = ")]}";
const typeRegex = /^ *typedef +.+ +([^ ]+) *; *$/;
const constantRegex = /^ *#define +([^ ]+) +.+$/;
const variableRegex = /^ *(.+) +([^ ]+) *; *$/;

const fileAnnotationsMap = {};
const fileDefinitionsMap = {};

class DocError extends Error {
    
    constructor(message) {
        super(message);
        this.annotation = null;
    }
}

class Annotation {
    
    constructor(name, value, path, lineNumber) {
        this.name = name;
        this.value = value;
        this.path = path;
        this.lineNumber = lineNumber;
        this.children = [];
        this.definitionCode = null;
    }
    
    getChildren(name) {
        return this.children.filter((annotation) => annotation.name === name);
    }
    
    getChildValue(name) {
        const annotationList = this.getChildren(name);
        if (annotationList.length <= 0) {
            return null;
        }
        if (annotationList.length > 1) {
            throw new DocError(`Expected at most one "${name}" annotation.`);
        }
        return annotationList[0].value;
    }
}

class Definition {
    
    constructor(annotation) {
        this.annotation = annotation;
        this.name = this.annotation.value;
        this.description = this.annotation.getChildValue("DESC");
    }
}

class SimpleDefinition extends Definition {
    
    constructor(annotation, regex, regexNameIndex) {
        super(annotation);
        const definitionCode = this.annotation.definitionCode;
        if (definitionCode === null) {
            this.regexResult = null;
        } else {
            this.regexResult = definitionCode.match(regex);
            if (this.regexResult === null) {
                throw new DocError(`Invalid statement for "${this.annotation.name}" definition.`);
            }
        }
        if (this.name === null) {
            if (this.regexResult === null) {
                throw new DocError("Missing definition name.");
            }
            this.name = this.regexResult[regexNameIndex];
        }
    }
}

class TypeDefinition extends SimpleDefinition {
    
    constructor(annotation, regex, regexNameIndex) {
        super(annotation, typeRegex, 1);
    }
}

class TypedDefinition extends SimpleDefinition {
    
    constructor(annotation, regex, regexNameIndex) {
        super(annotation, regex, regexNameIndex);
        this.type = this.annotation.getChildValue("TYPE");
    }
}

class ConstantDefinition extends TypedDefinition {
    
    constructor(annotation) {
        super(annotation, constantRegex, 1);
    }
}

class VariableDefinition extends TypedDefinition {
    
    constructor(annotation) {
        super(annotation, variableRegex, 2);
        if (this.type === null) {
            if (this.regexResult === null) {
                throw new DocError("Missing definition type.");
            }
            this.type = this.regexResult[1];
        }
    }
}

const definitionConstructorSet = {
    TYPE: TypeDefinition,
    CONST: ConstantDefinition,
    VAR: VariableDefinition,
};

function getCommentDepth(line) {
    let depth = 0;
    let index = 0;
    while (index < line.length) {
        if (line.substring(index, index + commentPrefix.length) === commentPrefix) {
            return depth;
        }
        if (line.substring(index, index + commentIndentation.length) !== commentIndentation) {
            return null;
        }
        depth += 1;
        index += commentIndentation.length;
    }
    return null;
}

function getEnclosureDepthOffset(text) {
    let output = 0;
    for (let index = 0; index < text.length; index += 1) {
        const tempCharacter = text.charAt(index);
        if (openEnclosureCharacterSet.indexOf(tempCharacter) >= 0) {
            output += 1;
        }
        if (closeEnclosureCharacterSet.indexOf(tempCharacter) >= 0) {
            output -= 1;
        }
    }
    return output;
}

function readAnnotation(path, lineList, startIndex) {
    const firstLine = lineList[startIndex];
    const firstDepth = getCommentDepth(firstLine);
    const trimmedFirstLine = firstLine.trim();
    const nameStartIndex = commentPrefix.length;
    let nameEndIndex = trimmedFirstLine.indexOf(" ", nameStartIndex);
    let tempValue;
    if (nameEndIndex >= 0) {
        tempValue = trimmedFirstLine.substring(nameEndIndex + 1, trimmedFirstLine.length);
    } else {
        nameEndIndex = trimmedFirstLine.length;
        tempValue = null;
    }
    const tempName = trimmedFirstLine.substring(nameStartIndex, nameEndIndex);
    let index = startIndex + 1;
    const annotation = new Annotation(tempName, tempValue, path, index);
    while (index < lineList.length) {
        const tempLine = lineList[index];
        const tempDepth = getCommentDepth(tempLine);
        if (tempDepth === null || tempDepth <= firstDepth) {
            break;
        }
        const tempResult = readAnnotation(path, lineList, index);
        index = tempResult.index;
        annotation.children.push(tempResult.annotation);
    }
    return {index, annotation};
}

function readDefinitionCode(lineList, startIndex) {
    if (startIndex >= lineList.length) {
        return null;
    }
    const firstLine = lineList[startIndex];
    if (firstLine.length <= 0 || getCommentDepth(firstLine) !== null) {
        return null;
    }
    const definitionLineList = [];
    let enclosureDepth = 0;
    let index = startIndex;
    while (index < lineList.length) {
        const tempLine = lineList[index];
        enclosureDepth += getEnclosureDepthOffset(tempLine);
        definitionLineList.push(tempLine);
        index += 1;
        if (enclosureDepth <= 0) {
            break;
        }
    }
    const definitionCode = definitionLineList.join(" ");
    return {index, definitionCode};
}

function readSourceFile(path) {
    const tempExtension = pathUtils.extname(path);
    if (tempExtension !== ".h" && tempExtension !== ".pppd") {
        return;
    }
    const fileContent = fs.readFileSync(path, "utf8");
    const lineList = fileContent.split("\n");
    const shortPath = pathUtils.relative(sourcePath, path);
    const annotationList = [];
    let index = 0;
    while (index < lineList.length) {
        const tempLine = lineList[index];
        if (getCommentDepth(tempLine) === 0) {
            const tempResult1 = readAnnotation(shortPath, lineList, index);
            index = tempResult1.index;
            const tempAnnotation = tempResult1.annotation;
            const tempResult2 = readDefinitionCode(lineList, index);
            if (tempResult2 !== null) {
                index = tempResult2.index;
                tempAnnotation.definitionCode = tempResult2.definitionCode;
            }
            annotationList.push(tempAnnotation);
        } else {
            index += 1;
        }
    }
    if (annotationList.length > 0) {
        fileAnnotationsMap[shortPath] = annotationList;
    }
}

function iterateOverDirectory(path) {
    const nameList = fs.readdirSync(path);
    nameList.forEach((name) => {
        const tempPath = pathUtils.join(path, name);
        const tempStats = fs.lstatSync(tempPath);
        if (tempStats.isDirectory()) {
            iterateOverDirectory(tempPath);
        } else {
            readSourceFile(tempPath);
        }
    });
}

function createDefinitions() {
    Object.keys(fileAnnotationsMap).forEach((path) => {
        const annotationList = fileAnnotationsMap[path];
        const definitionList = annotationList.map((annotation) => {
            try {
                const annotationName = annotation.name;
                const tempConstructor = definitionConstructorSet[annotationName];
                if (typeof tempConstructor === "undefined") {
                    throw new DocError(`Invalid annotation name "${annotationName}".`);
                }
                return new tempConstructor(annotation);
            } catch(error) {
                if (error instanceof DocError) {
                    error.annotation = annotation;
                }
                throw error;
            }
        });
        fileDefinitionsMap[path] = definitionList;
    });
}

try {
    
    console.log("Reading source files...");
    
    iterateOverDirectory(sourcePath);
    
    console.log("Generating documentation...");
    
    createDefinitions();
    console.log(fileDefinitionsMap);
    
    console.log("Finished.");
    
} catch (error) {
    if (error instanceof DocError) {
        console.log(`Error on line ${error.annotation.lineNumber} in ${error.annotation.path}: ${error.message}`);
    }
}


