
const fs = require("fs");

function isFirstIdentifierCharacter(characterCode) {
    return ((characterCode >= 65 && characterCode <= 90)
        || (characterCode >= 97 && characterCode <= 122)
        || characterCode === 95);
}

function isIdentifierCharacter(characterCode) {
    return (isFirstIdentifierCharacter(characterCode)
        || (characterCode >= 48 && characterCode <= 57));
}

function findNextIdentifier(text, index) {
    while (index < text.length) {
        const startIndex = index;
        const firstCharacterCode = text.charCodeAt(startIndex);
        if (!isFirstIdentifierCharacter(firstCharacterCode)) {
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
        return { startIndex, endIndex };
    }
    return null;
}

function convertIdentifierToRegex(identifier) {
    // Escape identifier text for use in a regex.
    // https://stackoverflow.com/questions/3561493/is-there-a-regexp-escape-function-in-javascript
    const regexText = identifier.replace(/[-\/\\^$*+?.()|[\]{}]/g, "\\$&")
    // Match the identifier surrounded by non-identifier characters.
    return new RegExp(`([^a-zA-Z0-9_]|^)(${regexText})([^a-zA-Z0-9_]|$)`, "g");
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
        return this.lineList.map(line => this.expandLine(line, argList));
    }
}

class Prepreprocessor {
    
    constructor(definitionsPath) {
        const tempContent = fs.readFileSync(definitionsPath, "utf8");
        const tempLineList = tempContent.split("\n");
        this.definitionList = [];
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
                    this.definitionList.push(currentDefinition);
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
    
    getDefinitionByName(name) {
        for (const definition of this.definitionList) {
            if (definition.name === name) {
                return definition;
            }
        }
        return null;
    }
    
    prepreprocessFile(sourcePath, destinationPath) {
        const tempContent = fs.readFileSync(sourcePath, "utf8");
        const sourceLineList = tempContent.split("\n");
        const destinationLineList = [];
        for (const line of sourceLineList) {
            const trimmedLine = line.trim();
            if (trimmedLine.startsWith("!!!")) {
                const tempTermList = trimmedLine.substring(3, trimmedLine.length).split(" ");
                const tempName = tempTermList[0];
                const tempDefinition = this.getDefinitionByName(tempName);
                if (tempDefinition === null) {
                    throw new Error(`Unknown prepreprocessor definition "${tempName}".`);
                }
                const tempArgList = tempTermList.slice(1, tempTermList.length);
                const tempLineList = tempDefinition.expand(tempArgList);
                for (const tempLine of tempLineList) {
                    destinationLineList.push(tempLine);
                }
            } else {
                destinationLineList.push(line);
            }
        }
        fs.writeFileSync(destinationPath, destinationLineList.join("\n"));
    }
}

module.exports = {
    Prepreprocessor: Prepreprocessor
};


