
const fs = require("fs");
const pathUtils = require("path");

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
                    return null;
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

function iterateOverDirectory(path, handle) {
    const nameList = fs.readdirSync(path);
    nameList.forEach((name) => {
        const tempPath = pathUtils.join(path, name);
        const tempStats = fs.lstatSync(tempPath);
        if (tempStats.isDirectory()) {
            iterateOverDirectory(tempPath, handle);
        } else {
            handle(tempPath);
        }
    });
}

module.exports = {
    findMatchingStartParenthesis,
    findMatchingEndParenthesis,
    safeSplit,
    iterateOverDirectory,
};


