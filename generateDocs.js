
const fs = require("fs");
const pathUtils = require("path");

const sourcePath = pathUtils.join(__dirname, "src");
const templatePath = pathUtils.join(__dirname, "docTemplate.html");
const documentPath = pathUtils.join(__dirname, "cImplDoc.html");
const commentPrefix = "///";
const commentIndentation = "    ";
const openEnclosureCharacterSet = "([{";
const closeEnclosureCharacterSet = ")]}";
const typeRegex = /^ *typedef +.+ +([^ ]+) *; *$/;
const constantRegex = /^ *#define +([^ ]+) +.+$/;
const variableRegex = /^ *(.*[^ ]) +([^ ]+) *; *$/;
const structRegex = /^ *typedef +struct +.+ +{(.+)} +([^ ]+) *; *$/;
const definitionMemberRegex = /^ *(.*[^ ]) +([^ ]+) *$/;
const preprocessorMacroRegex = /^#define *([^ ]+) *\((.*)\) *.*$/;
const prepreprocessorMacroRegex = /^DEFINE ([^ ]+) ?(.*)$/;

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
    
    // Concrete subclasses of Definition must implement these methods:
    // getClassDisplayName
    
    constructor(annotation) {
        this.annotation = annotation;
        this.name = this.annotation.value;
        this.description = this.annotation.getChildValue("DESC");
    }
    
    getBulletHtmlList() {
        return [];
    }
    
    convertToHtml() {
        const htmlList = [`<p>${this.getClassDisplayName()} <span class="code">${this.name}</span></p><div class="description">`];
        if (this.description !== null) {
            htmlList.push(`<p>${this.description}</p>`);
        }
        const bulletHtmlList = this.getBulletHtmlList();
        if (bulletHtmlList.length > 0) {
            htmlList.push(`<ul>`);
            bulletHtmlList.forEach((bulletHtml) => {
                htmlList.push(bulletHtml);
            });
            htmlList.push(`</ul>`);
        }
        htmlList.push(`</div>`);
        return htmlList.join("\n");
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
    
    getClassDisplayName() {
        return "Type";
    }
}

class TypedDefinition extends SimpleDefinition {
    
    constructor(annotation, regex, regexNameIndex) {
        super(annotation, regex, regexNameIndex);
        this.type = this.annotation.getChildValue("TYPE");
    }
    
    getBulletHtmlList() {
        const output = super.getBulletHtmlList();
        if (this.type !== null) {
            output.push(`<li>Type = <span class="code">${this.type}</span></li>`);
        }
        return output;
    }
}

class ConstantDefinition extends TypedDefinition {
    
    constructor(annotation) {
        super(annotation, constantRegex, 1);
    }
    
    getClassDisplayName() {
        return "Constant";
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
    
    getClassDisplayName() {
        return "Global variable";
    }
}

class DefinitionMember {
    
    constructor(name) {
        this.name = name;
        this.type = null;
        this.description = null;
    }
}

class StructDefinition extends SimpleDefinition {
    
    constructor(annotation, regex, regexNameIndex) {
        super(annotation, structRegex, 2);
        this.fields = [];
        if (this.regexResult !== null) {
            const fieldCodeList = safeSplit(this.regexResult[1], ";");
            parseTypedDefinitionMembers(this.fields, fieldCodeList);
        }
        const fieldAnnotationList = this.annotation.getChildren("FIELD");
        populateMemberDefinitions(this.fields, fieldAnnotationList);
    }
    
    getClassDisplayName() {
        return "Data structure";
    }
}

class FunctionDefinition extends Definition {
    
    constructor(annotation) {
        super(annotation);
        this.args = [];
        this.returnType = null;
        this.readFunctionDefinitionCode();
        const argAnnotationList = this.annotation.getChildren("ARG");
        populateMemberDefinitions(this.args, argAnnotationList);
        const tempType = this.annotation.getChildValue("RET");
        if (tempType !== null) {
            this.returnType = tempType;
        }
    }
    
    getClassDisplayName() {
        return "Function";
    }
    
    readFunctionDefinitionCode() {
        const definitionCode = this.annotation.definitionCode;
        if (definitionCode === null) {
            return;
        }
        let argNameList = null;
        let tempResult = definitionCode.match(preprocessorMacroRegex);
        if (tempResult !== null) {
            this.name = tempResult[1];
            argNameList = tempResult[2].split(",");
        } else {
            tempResult = definitionCode.match(prepreprocessorMacroRegex);
            if (tempResult !== null) {
                this.name = tempResult[1];
                argNameList = tempResult[2].split(" ");
            }
        }
        if (argNameList !== null) {
            argNameList.forEach((name) => {
                name = name.trim();
                if (name.length <= 0) {
                    return;
                }
                this.args.push(new DefinitionMember(name));
            });
            return;
        }
        const endParenthesisIndex = definitionCode.lastIndexOf(")");
        if (endParenthesisIndex < 0) {
            return;
        }
        const startParenthesisIndex = findMatchingStartParenthesis(
            definitionCode,
            endParenthesisIndex,
        );
        if (startParenthesisIndex < 0) {
            return;
        }
        const tempText = definitionCode.substring(0, startParenthesisIndex);
        tempResult = tempText.match(definitionMemberRegex);
        if (tempResult === null) {
            return;
        }
        const argCodeList = safeSplit(definitionCode.substring(
            startParenthesisIndex + 1,
            endParenthesisIndex,
        ), ",");
        this.name = tempResult[2];
        parseTypedDefinitionMembers(this.args, argCodeList);
        this.returnType = tempResult[1];
    }
}

const definitionConstructorSet = {
    TYPE: TypeDefinition,
    CONST: ConstantDefinition,
    VAR: VariableDefinition,
    STRUCT: StructDefinition,
    FUNC: FunctionDefinition,
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

function findMatchingStartParenthesis(text, endParenthesisIndex) {
    let depth = 1;
    for (let index = endParenthesisIndex - 1; index >= 0; index -= 1) {
        const tempCharacter = text.charAt(index);
        if (tempCharacter === "(") {
            depth -= 1;
            if (depth === 0) {
                return index;
            }
        } else if (tempCharacter === ")") {
            depth += 1;
        }
    }
    return -1;
}

function findMatchingEndParenthesis(text, startParenthesisIndex) {
    let depth = 1;
    for (let index = startParenthesisIndex + 1; index < text.length; index += 1) {
        const tempCharacter = text.charAt(index);
        if (tempCharacter === ")") {
            depth -= 1;
            if (depth === 0) {
                return index;
            }
        } else if (tempCharacter === "(") {
            depth += 1;
        }
    }
    return -1;
}

function safeSplit(text, delimiter) {
    const output = [];
    let termStartIndex = 0;
    let index = 0;
    while (true) {
        let lastIndex = index;
        let shouldAddTerm = false;
        let shouldBreak = false;
        if (index >= text.length) {
            shouldAddTerm = true;
            shouldBreak = true;
        } else if (text.substring(index, index + delimiter.length) === delimiter) {
            index += delimiter.length;
            shouldAddTerm = true;
        } else {
            const tempCharacter = text.charAt(index);
            if (tempCharacter === "(") {
                index = findMatchingEndParenthesis(text, index);
                if (index < 0) {
                    throw new DocError("Missing close parenthesis.");
                }
            }
            index += 1;
        }
        if (shouldAddTerm) {
            const tempTerm = text.substring(termStartIndex, lastIndex);
            output.push(tempTerm);
            termStartIndex = index;
        }
        if (shouldBreak) {
            break;
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

function parseTypedDefinitionMembers(destination, memberCodeList) {
    memberCodeList.forEach((memberCode) => {
        const tempResult = memberCode.match(definitionMemberRegex);
        if (tempResult !== null) {
            const tempMember = new DefinitionMember(tempResult[2]);
            tempMember.type = tempResult[1];
            destination.push(tempMember);
        }
    });
}

function populateMemberDefinitions(memberDefinitionList, annotationList) {
    annotationList.forEach((annotation) => {
        let tempMember = memberDefinitionList.find((memberDefinition) => (
            memberDefinition.name === annotation.value
        ));
        if (typeof tempMember === "undefined") {
            tempMember = new DefinitionMember(annotation.value);
            memberDefinitionList.push(tempMember);
        }
        tempMember.description = annotation.getChildValue("DESC");
        const tempType = annotation.getChildValue("TYPE");
        if (tempType !== null) {
            tempMember.type = tempType;
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

function populateTemplatePlaceholders(templateText, replacementMap) {
    let output = templateText;
    Object.keys(replacementMap).forEach((name) => {
        output = output.replace(`{${name}}`, replacementMap[name]);
    });
    return output;
}

function formatHtmlStyle(documentHtml) {
    const oldHtmlList = documentHtml.split("`");
    const newHtmlList = [];
    oldHtmlList.forEach((htmlSnippet, index) => {
        if (index > 0) {
            if (index % 2 == 1) {
                newHtmlList.push(`<span class="code">`);
            } else {
                newHtmlList.push(`</span>`);
            }
        }
        newHtmlList.push(htmlSnippet);
    });
    return newHtmlList.join("");
}

function createDocument() {
    const templateText = fs.readFileSync(templatePath, "utf8");
    const tempDate = new Date();
    const dateText = tempDate.toLocaleDateString("en-US", {
        year: "numeric",
        month: "numeric",
        day: "numeric",
        hour: "numeric",
        minute: "2-digit",
        timeZoneName: "short",
    });
    const definitionsHtml = Object.keys(fileDefinitionsMap).map((path) => {
        const htmlList = [`<p class="pathTitle">${path}</p>`];
        const definitionList = fileDefinitionsMap[path];
        definitionList.forEach((definition) => {
            htmlList.push(definition.convertToHtml());
        });
        return htmlList.join("\n");
    }).join("\n");
    let documentHtml = populateTemplatePlaceholders(templateText, {
        TIMESTAMP: dateText,
        DEFINITIONS: definitionsHtml,
    });
    documentHtml = formatHtmlStyle(documentHtml);
    fs.writeFileSync(documentPath, documentHtml);
}

try {
    
    console.log("Reading source files...");
    
    iterateOverDirectory(sourcePath);
    
    console.log("Generating documentation...");
    
    createDefinitions();
    createDocument();
    
    console.log("Finished.");
    
} catch (error) {
    if (error instanceof DocError) {
        console.log(`Error on line ${error.annotation.lineNumber} in ${error.annotation.path}: ${error.message}`);
    } else {
        throw(error);
    }
}


