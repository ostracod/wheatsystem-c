
const fs = require("fs");

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
        this.argRegexList = argNameList.map(convertIdentifierToRegex);
        this.lineList = [];
    }
    
    expandLine(line, argList) {
        if (argList.length !== this.argRegexList.length) {
            throw new Error("Unexpected number of arguments for  prepreprocessor definition.");
        }
        for (let index = 0; index < argList.length; index++) {
            const tempArg = argList[index];
            const tempRegex = this.argRegexList[index];
            line = line.replace(tempRegex, (match, group1, group2, group3) => {
                return group1 + tempArg + group3;
            });
        }
        return line;
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


