
const fs = require("fs");
const pathUtils = require("path");
const commonUtils = require("./commonUtils");

const sourcePath = pathUtils.join(__dirname, "src");
const templatePath = pathUtils.join(__dirname, "docTemplate.html");
const documentPath = pathUtils.join(__dirname, "cImplDoc.html");
const docExtensionSet = [".h", ".pppd", ".txt"];
const commentPrefix = "///";
const commentIndentation = "    ";
const openEnclosureCharacterSet = "([{";
const closeEnclosureCharacterSet = ")]}";
const typeRegex = /^ *typedef +.+ +([^ ]+) *; *$/;
const constantRegex = /^ *#define +([^ ]+) +.+$/;
const variableRegex = /^ *(.*[^ ]) +([^ ]+) *; *$/;
const structRegex = /^ *typedef +struct +.+ +{(.+)} +([^ ]+) *; *$/;
const definitionMemberRegex = /^ *(.*[^ ][ \*]+)([^ ]+) *$/;
const preprocessorMacroRegex = /^#define *([^ \(]+) *\(([^\)]*)\) *.*$/;
const prepreprocessorMacroRegex = /^DEFINE ([^ ]+) ?(.*)$/;

const fileAnnotationsMap = {};
const fileDescriptionMap = {};
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
    
    getTypeHtml() {
        return null
    }
    
    getMembersHtml() {
        return null
    }
    
    convertToHtml() {
        const htmlList = [`<p>${this.getClassDisplayName()} <span class="code">${this.name}</span></p><div class="description">`];
        const typeHtml = this.getTypeHtml();
        if (typeHtml !== null) {
            htmlList.push(typeHtml);
        }
        if (this.description !== null) {
            htmlList.push(`<p>${this.description}</p>`);
        }
        const membersHtml = this.getMembersHtml();
        if (membersHtml !== null) {
            htmlList.push(membersHtml);
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
    
    getTypeHtml() {
        return `<p>Type = ${getTypeHtml(this.type)}</p>`;
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
    
    convertToHtml() {
        const typeText = (this.type === null) ? ":(None)" : this.type;
        const typeHtml = getTypeHtml(typeText);
        const descriptionText = (this.description === null) ? "(None)" : this.description;
        return `<tr><td><span class="code">${this.name}</span></td><td>${typeHtml}</td><td>${descriptionText}</td></tr>`;
    }
}

class MemberDefinitionExtension {
    
    // Concrete subclasses of MemberDefinitionExtension must implement these methods:
    // readDefinitionCode, getMemberAnnotationName, getMemberDisplayName
    
    constructor(annotation) {
        this.annotation = annotation;
        this.members = [];
    }
    
    initialize() {
        this.readDefinitionCode();
        this.populateMemberDefinitions(this.getMemberAnnotationName());
    }
    
    populateMemberDefinitions(annotationName) {
        const annotationList = this.annotation.getChildren(annotationName);
        annotationList.forEach((annotation) => {
            let tempMember = this.members.find((memberDefinition) => (
                memberDefinition.name === annotation.value
            ));
            if (typeof tempMember === "undefined") {
                tempMember = new DefinitionMember(annotation.value);
                this.members.push(tempMember);
            }
            tempMember.description = annotation.getChildValue("DESC");
            const tempType = annotation.getChildValue("TYPE");
            if (tempType !== null) {
                tempMember.type = tempType;
            }
        });
    }
    
    parseTypedDefinitionMembers(memberCodeList) {
        memberCodeList.forEach((memberCode) => {
            const tempResult = memberCode.match(definitionMemberRegex);
            if (tempResult !== null) {
                let tempName = tempResult[2];
                const bracketIndex = tempName.indexOf("[");
                let bracketText;
                if (bracketIndex >= 0) {
                    bracketText = tempName.substring(bracketIndex, tempName.length);
                    tempName = tempName.substring(0, bracketIndex);
                } else {
                    bracketText = "";
                }
                const tempMember = new DefinitionMember(tempName);
                tempMember.type = tempResult[1].trim() + bracketText;
                this.members.push(tempMember);
            }
        });
    }
    
    getMembersHtml() {
        if (this.members.length <= 0) {
            return "";
        }
        const htmlList = [`<table><tr><th>${this.getMemberDisplayName()} name</th><th>Type</th><th>Description</th></tr>`];
        this.members.forEach((member) => {
            htmlList.push(member.convertToHtml());
        });
        htmlList.push(`</table>`);
        return htmlList.join("\n");
    }
}

class StructDefinitionExtension extends MemberDefinitionExtension {
    
    constructor(annotation, regexGroup) {
        super(annotation);
        this.regexGroup = regexGroup;
        this.initialize();
    }
    
    readDefinitionCode() {
        if (this.regexGroup !== null) {
            const fieldCodeList = commonUtils.safeSplit(this.regexGroup, ";");
            this.parseTypedDefinitionMembers(fieldCodeList);
        }
    }
    
    getMemberAnnotationName() {
        return "FIELD";
    }
    
    getMemberDisplayName() {
        return "Field";
    }
}

class FunctionDefinitionExtension extends MemberDefinitionExtension {
    
    constructor(annotation) {
        super(annotation);
        this.name = null;
        this.returnType = null;
        this.initialize();
        const tempType = this.annotation.getChildValue("RET");
        if (tempType !== null) {
            this.returnType = tempType;
        }
    }
    
    readDefinitionCode() {
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
                this.members.push(new DefinitionMember(name));
            });
            return;
        }
        const endParenthesisIndex = definitionCode.lastIndexOf(")");
        if (endParenthesisIndex < 0) {
            return;
        }
        const startParenthesisIndex = commonUtils.findMatchingStartParenthesis(
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
        const argCodeList = commonUtils.safeSplit(definitionCode.substring(
            startParenthesisIndex + 1,
            endParenthesisIndex,
        ), ",");
        this.name = tempResult[2];
        this.parseTypedDefinitionMembers(argCodeList);
        this.returnType = tempResult[1].trim();
    }
    
    getMemberAnnotationName() {
        return "ARG";
    }
    
    getMemberDisplayName() {
        return "Argument";
    }
    
    getMembersHtml() {
        if (this.members.length <= 0) {
            return `<p>This function does not accept any arguments.</p>`;
        }
        return super.getMembersHtml();
    }
}

class StructDefinition extends SimpleDefinition {
    
    constructor(annotation, regex, regexNameIndex) {
        super(annotation, structRegex, 2);
        const regexGroup = (this.regexResult === null) ? null : this.regexResult[1];
        this.extension = new StructDefinitionExtension(annotation, regexGroup);
    }
    
    getClassDisplayName() {
        return "Data structure";
    }
    
    getMembersHtml() {
        return this.extension.getMembersHtml();
    }
}

class FunctionDefinition extends Definition {
    
    constructor(annotation) {
        super(annotation);
        this.extension = new FunctionDefinitionExtension(annotation);
        if (this.name === null) {
            this.name = this.extension.name;
        }
    }
    
    getClassDisplayName() {
        return "Function";
    }
    
    getTypeHtml() {
        return `<p>Return type = ${getTypeHtml(this.extension.returnType)}</p>`;
    }
    
    getMembersHtml() {
        return this.extension.getMembersHtml();
    }
}

class FunctionTypeDefinition extends FunctionDefinition {
    
    getClassDisplayName() {
        return "Function type";
    }
}

const definitionConstructorSet = {
    TYPE: TypeDefinition,
    CONST: ConstantDefinition,
    VAR: VariableDefinition,
    STRUCT: StructDefinition,
    FUNC: FunctionDefinition,
    FUNC_TYPE: FunctionTypeDefinition,
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

function safeSplit(text, delimiter) {
    const output = commonUtils.safeSplit(text, delimiter);
    if (output === null) {
        throw new DocError("Missing close parenthesis.");
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
    if (!docExtensionSet.includes(tempExtension)) {
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

function processAnnotations() {
    Object.keys(fileAnnotationsMap).forEach((path) => {
        const annotationList = fileAnnotationsMap[path];
        const definitionList = []
        annotationList.forEach((annotation) => {
            const annotationName = annotation.name;
            if (annotationName === "DESC") {
                fileDescriptionMap[path] = annotation.value;
                return;
            }
            try {
                const tempConstructor = definitionConstructorSet[annotationName];
                if (typeof tempConstructor === "undefined") {
                    throw new DocError(`Invalid annotation name "${annotationName}".`);
                }
                const tempDefinition = new tempConstructor(annotation);
                definitionList.push(tempDefinition);
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

function getTypeHtml(typeText) {
    if (typeText === null) {
        return "(None)";
    } else if (typeText.charAt(0) === ":") {
        return typeText.substring(1, typeText.length);
    } else {
        return `<span class="code">${typeText}</span>`;
    }
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
        const tempDescription = fileDescriptionMap[path];
        if (typeof tempDescription !== 'undefined') {
            htmlList.push(`<p>${tempDescription}</p>`);
        }
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
    
    commonUtils.iterateOverDirectory(sourcePath, readSourceFile);
    
    console.log("Generating documentation...");
    
    processAnnotations();
    createDocument();
    
    console.log("Finished. Documentation path:");
    console.log(documentPath);
    
} catch (error) {
    if (error instanceof DocError) {
        console.log(`Error on line ${error.annotation.lineNumber} in ${error.annotation.path}: ${error.message}`);
    } else {
        throw(error);
    }
}


