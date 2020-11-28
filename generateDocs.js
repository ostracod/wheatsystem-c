
const fs = require("fs");
const pathUtils = require("path");

const sourcePath = pathUtils.join(__dirname, "src");
const commentPrefix = "///";
const commentIndentation = "    ";
const openEnclosureCharacterSet = "([{";
const closeEnclosureCharacterSet = ")]}";

const fileAnnotationsMap = {};

class Annotation {
    
    constructor(name, value) {
        this.name = name;
        this.value = value;
        this.children = [];
        this.definitionCode = null;
    }
}

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

function readAnnotation(lineList, startIndex) {
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
    const annotation = new Annotation(tempName, tempValue);
    let index = startIndex + 1;
    while (index < lineList.length) {
        const tempLine = lineList[index];
        const tempDepth = getCommentDepth(tempLine);
        if (tempDepth === null || tempDepth <= firstDepth) {
            break;
        }
        const tempResult = readAnnotation(lineList, index);
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
    const definitionCode = definitionLineList.join("\n");
    return {index, definitionCode};
}

function readSourceFile(path) {
    const tempExtension = pathUtils.extname(path);
    if (tempExtension !== ".h" && tempExtension !== ".pppd") {
        return;
    }
    const fileContent = fs.readFileSync(path, "utf8");
    const lineList = fileContent.split("\n");
    const annotationList = [];
    let index = 0;
    while (index < lineList.length) {
        const tempLine = lineList[index];
        if (getCommentDepth(tempLine) === 0) {
            const tempResult1 = readAnnotation(lineList, index);
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
        fileAnnotationsMap[path] = annotationList;
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

console.log("Reading source files...");

iterateOverDirectory(sourcePath);

console.log("Generating documentation...");

// TODO: Generate documentation.
console.log(JSON.stringify(fileAnnotationsMap, null, 4));

console.log("Finished.");


