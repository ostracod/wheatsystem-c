
const fs = require("fs");
const pathUtils = require("path");

const sourcePath = pathUtils.join(__dirname, "src");
const commentPrefix = "///";
const commentIndentation = "    ";

const fileAnnotationsMap = {};

class Annotation {
    
    constructor(name, value) {
        this.name = name;
        this.value = value;
        this.children = [];
        this.definition = null;
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
    let annotationPrecedesDefinition = false;
    let index = startIndex + 1;
    while (index < lineList.length) {
        const tempLine = lineList[index];
        const tempDepth = getCommentDepth(tempLine);
        if (tempDepth === null) {
            annotationPrecedesDefinition = (tempLine.trim().length > 0);
            break;
        }
        if (tempDepth <= firstDepth) {
            break;
        }
        const tempResult = readAnnotation(lineList, index);
        index = tempResult.index;
        annotation.children.push(tempResult.annotation);
    }
    if (annotationPrecedesDefinition) {
        // TODO: Handle multi-line definitions.
        annotation.definition = lineList[index];
        index += 1;
    }
    return {index, annotation};
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
            const tempResult = readAnnotation(lineList, index);
            index = tempResult.index;
            annotationList.push(tempResult.annotation);
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


