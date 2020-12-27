
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

class PrepreprocessorDefinition {
    
    constructor(name, argNameList) {
        this.name = name;
        this.argNameList = argNameList;
        this.argIndexMap = {};
        argNameList.forEach((argName, argIndex) => {
            this.argIndexMap[argName] = argIndex;
        });
        this.lineList = [];
    }
    
    expandLine(line, argList) {
        if (argList.length !== this.argNameList.length) {
            throw new Error("Unexpected number of arguments for  prepreprocessor definition.");
        }
        const textList = [];
        let index = 0;
        while (true) {
            const tempResult = findNextIdentifier(line, index);
            if (tempResult === null) {
                textList.push(line.substring(index, line.length));
                break;
            }
            const { startIndex, endIndex } = tempResult;
            const tempIdentifier = line.substring(startIndex, endIndex);
            const argIndex = this.argIndexMap[tempIdentifier];
            if (typeof argIndex !== "undefined") {
                textList.push(line.substring(index, startIndex));
                textList.push(argList[argIndex]);
            } else {
                textList.push(line.substring(index, endIndex));
            }
            index = endIndex;
        }
        return textList.join("");
    }
    
    expand(argList) {
        return this.lineList.map(line => this.expandLine(line, argList)).join("\n");
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
                } else {
                    currentDefinition.lineList.push(line);
                }
            }
        }
    }
    
    prepreprocessFile(sourcePath, destinationPath) {
        const sourceContent = fs.readFileSync(sourcePath, "utf8");
        const textList = [];
        let index = 0;
        while (true) {
            const lastIndex = index;
            const tempResult = findNextIdentifier(sourceContent, index);
            if (tempResult === null) {
                textList.push(sourceContent.substring(lastIndex, sourceContent.length));
                break;
            }
            const { startIndex, endIndex } = tempResult;
            const tempIdentifier = sourceContent.substring(startIndex, endIndex);
            const tempDefinition = this.definitionMap[tempIdentifier];
            if (typeof tempDefinition === "undefined") {
                textList.push(sourceContent.substring(lastIndex, endIndex));
                index = endIndex;
                continue;
            }
            let hasFoundParenthesis = false;
            index = endIndex;
            while (index < sourceContent.length) {
                const characterCode = sourceContent.charCodeAt(index);
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
                textList.push(sourceContent.substring(lastIndex, index));
                continue;
            }
            const startParenthesisIndex = index;
            const endParenthesisIndex = commonUtils.findMatchingEndParenthesis(
                sourceContent,
                startParenthesisIndex,
            );
            if (endParenthesisIndex < 0) {
                throw new Error(`Missing close parenthesis in file: ${sourcePath}`);
            }
            const argsText = sourceContent.substring(
                startParenthesisIndex + 1,
                endParenthesisIndex,
            );
            let argList = commonUtils.safeSplit(argsText, ",");
            if (argList === null) {
                throw new Error(`Could not parse file: ${sourcePath}`);
            }
            argList = argList.map((arg) => arg.trim());
            const expandedDefinition = tempDefinition.expand(argList);
            textList.push(sourceContent.substring(lastIndex, startIndex));
            textList.push(expandedDefinition);
            index = endParenthesisIndex + 1;
        }
        fs.writeFileSync(destinationPath, textList.join(""));
    }
}

module.exports = {
    Prepreprocessor: Prepreprocessor
};


