
const fs = require("fs");
const commonUtils = require("./commonUtils");

function isFirstIdentifierCharacter(characterCode) {
    return ((characterCode >= 65 && characterCode <= 90)
        || (characterCode >= 97 && characterCode <= 122)
        || characterCode === 95);
}

function isDigitCharacter(characterCode) {
    return (characterCode >= 48 && characterCode <= 57)
}

function isIdentifierCharacter(characterCode) {
    return (isFirstIdentifierCharacter(characterCode)
        || isDigitCharacter(characterCode));
}

function findNextIdentifier(text, index) {
    while (index < text.length) {
        const startIndex = index;
        const firstCharacterCode = text.charCodeAt(startIndex);
        const isNumber = isDigitCharacter(firstCharacterCode);
        if (!isNumber && !isFirstIdentifierCharacter(firstCharacterCode)) {
            index += 1;
            continue;
        }
        let endIndex = startIndex + 1;
        while (endIndex < text.length) {
            const characterCode = text.charCodeAt(endIndex);
            if (!isIdentifierCharacter(characterCode)) {
                break;
            }
            endIndex += 1;
        }
        if (!isNumber) {
            return { startIndex, endIndex };
        }
        index = endIndex;
    }
    return null;
}

function removeComments(fileContent) {
    const lineList = fileContent.split("\n");
    for (let index = 0; index < lineList.length; index++) {
        const tempLine = lineList[index];
        const commentIndex = tempLine.indexOf("//");
        if (commentIndex < 0) {
            continue;
        }
        lineList[index] = tempLine.substring(0, commentIndex);
    }
    return lineList.join("\n");
}

class PrepreprocessorDefinition {
    
    constructor(name, argNameList) {
        this.name = name;
        this.argNameList = argNameList;
        this.argIndexMap = {};
        argNameList.forEach((argName, argIndex) => {
            this.argIndexMap[argName] = argIndex;
        });
        this.bodyLineList = [];
        this.headerLineList = [];
        this.body = null;
        this.header = null;
        this.invocationMap = {};
    }
    
    initialize() {
        this.body = this.bodyLineList.join("\n");
        if (this.headerLineList.length > 0) {
            this.header = this.headerLineList.join("\n");
        } else {
            this.header = null;
        }
    }
    
    expand(inputText, argList) {
        if (argList.length !== this.argNameList.length) {
            throw new Error("Unexpected number of arguments for  prepreprocessor definition.");
        }
        this.invocationMap[argList.join(",")] = argList;
        const textList = [];
        let index = 0;
        while (true) {
            const tempResult = findNextIdentifier(inputText, index);
            if (tempResult === null) {
                textList.push(inputText.substring(index, inputText.length));
                break;
            }
            const { startIndex, endIndex } = tempResult;
            const tempIdentifier = inputText.substring(startIndex, endIndex);
            const argIndex = this.argIndexMap[tempIdentifier];
            if (typeof argIndex !== "undefined") {
                textList.push(inputText.substring(index, startIndex));
                textList.push(argList[argIndex]);
            } else {
                textList.push(inputText.substring(index, endIndex));
            }
            index = endIndex;
        }
        const expandedText = textList.join("");
        return expandedText.split("###").join("");
    }
    
    expandBody(argList) {
        return this.expand(this.body, argList);
    }
    
    expandHeader(argList) {
        return this.expand(this.header, argList);
    }
    
    getHeaders() {
        if (this.header === null) {
            return [];
        }
        return Object.keys(this.invocationMap).map((key) => {
            const argList = this.invocationMap[key];
            return this.expandHeader(argList);
        });
    }
}

class Prepreprocessor {
    
    constructor() {
        // Map from name to PrepreprocessorDefinition.
        this.definitionMap = {};
    }
    
    readDefinitionsFile(definitionsPath) {
        const tempContent = fs.readFileSync(definitionsPath, "utf8");
        const tempLineList = tempContent.split("\n");
        let currentDefinition = null;
        let isInHeader = false;
        for (const line of tempLineList) {
            if (line.length <= 0) {
                continue;
            }
            if (line.trim().startsWith("//")) {
                continue;
            }
            if (currentDefinition === null) {
                const tempTermList = line.split(" ");
                let tempDirective = tempTermList[0];
                if (tempDirective === "DEFINE") {
                    if (tempTermList.length < 2) {
                        throw new Error("Malformed prepreprocessor definition.");
                    }
                    currentDefinition = new PrepreprocessorDefinition(
                        tempTermList[1],
                        tempTermList.slice(2, tempTermList.length)
                    );
                    this.definitionMap[currentDefinition.name] = currentDefinition;
                } else {
                    throw new Error(`Unexpected prepreprocessor directive "${tempDirective}".`);
                }
            } else {
                if (line === "END") {
                    currentDefinition = null;
                    isInHeader = false;
                } else if (line === "UNIQUE_HEADER") {
                    isInHeader = true;
                } else if (isInHeader) {
                    currentDefinition.headerLineList.push(line);
                } else {
                    currentDefinition.bodyLineList.push(line);
                }
            }
        }
    }
    
    initializeDefinitions() {
        for (const name in this.definitionMap) {
            const tempDefinition = this.definitionMap[name];
            tempDefinition.initialize();
        }
    }
    
    prepreprocessFileHelper(fileContent) {
        const textList = [];
        let expansionCount = 0;
        let index = 0;
        while (true) {
            const lastIndex = index;
            const tempResult = findNextIdentifier(fileContent, index);
            if (tempResult === null) {
                textList.push(fileContent.substring(lastIndex, fileContent.length));
                break;
            }
            const { startIndex, endIndex } = tempResult;
            const tempIdentifier = fileContent.substring(startIndex, endIndex);
            const tempDefinition = this.definitionMap[tempIdentifier];
            if (typeof tempDefinition === "undefined") {
                textList.push(fileContent.substring(lastIndex, endIndex));
                index = endIndex;
                continue;
            }
            let hasFoundParenthesis = false;
            index = endIndex;
            while (index < fileContent.length) {
                const characterCode = fileContent.charCodeAt(index);
                if (characterCode === 40) {
                    hasFoundParenthesis = true;
                    break;
                }
                if (characterCode !== 32) {
                    break;
                }
                index += 1;
            }
            if (!hasFoundParenthesis) {
                textList.push(fileContent.substring(lastIndex, index));
                continue;
            }
            const startParenthesisIndex = index;
            const endParenthesisIndex = commonUtils.findMatchingEndParenthesis(
                fileContent,
                startParenthesisIndex,
            );
            if (endParenthesisIndex < 0) {
                throw new Error(`Missing close parenthesis in file: ${sourcePath}`);
            }
            const argsText = fileContent.substring(
                startParenthesisIndex + 1,
                endParenthesisIndex,
            );
            let argList = commonUtils.safeSplit(argsText, ",");
            if (argList === null) {
                throw new Error(`Could not parse file: ${sourcePath}`);
            }
            argList = argList.map((arg) => arg.trim());
            const expandedDefinition = tempDefinition.expandBody(argList);
            textList.push(fileContent.substring(lastIndex, startIndex));
            textList.push(expandedDefinition);
            expansionCount += 1;
            index = endParenthesisIndex + 1;
        }
        return {
            expansionCount,
            fileContent: textList.join(""),
        };
    }
    
    prepreprocessFile(sourcePath, destinationPath) {
        let fileContent = fs.readFileSync(sourcePath, "utf8");
        fileContent = removeComments(fileContent);
        while (true) {
            const tempResult = this.prepreprocessFileHelper(fileContent);
            if (tempResult.expansionCount <= 0) {
                break;
            }
            fileContent = tempResult.fileContent;
        }
        fs.writeFileSync(destinationPath, fileContent);
    }
    
    getHeaders() {
        const output = [];
        for (const name in this.definitionMap) {
            const tempDefinition = this.definitionMap[name];
            const headerList = tempDefinition.getHeaders();
            for (const header of headerList) {
                output.push(header);
            }
        }
        return output;
    }
}

module.exports = {
    Prepreprocessor: Prepreprocessor
};


