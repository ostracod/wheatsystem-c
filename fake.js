
const fs = require("fs");
const pathUtils = require("path");
const childProcess = require("child_process");
const commonUtils = require("./commonUtils");
const Prepreprocessor = require("./prepreprocessor").Prepreprocessor;

const sourcePath = pathUtils.join(__dirname, "src");
const commonPath = pathUtils.join(sourcePath, "common");
const platformsPath = pathUtils.join(sourcePath, "platforms");
const intermediatePath = pathUtils.join(__dirname, "intermediate");
const buildPath = pathUtils.join(__dirname, "build");
const bytecodeInstructionsPath = pathUtils.join(__dirname, "..", "wheatsystem-spec", "bytecodeInstructions.json");

const sourceExtensionSet = [".pppd", ".h", ".c"];
const fileCategoryInclusionOrder = [
    "ints",
    "generics",
    "structs",
    "constants",
    "vars",
    "funcs",
];

const platformDefinitionsPath = pathUtils.join(__dirname, "platformDefinitions.json");
const platformDefinitionList = JSON.parse(fs.readFileSync(platformDefinitionsPath), "utf8");

class BuildFile {
    constructor(category, sourcePath, intermediatePath) {
        this.categoryIndex = fileCategoryInclusionOrder.indexOf(category);
        this.sourcePath = sourcePath;
        this.intermediatePath = intermediatePath;
        const tempExtension = pathUtils.extname(this.intermediatePath);
        buildFileList.push(this);
        if (tempExtension === ".pppd") {
            prepreprocessorFileList.push(this);
        } else if (tempExtension === ".h") {
            headerFileList.push(this);
        } else if (tempExtension === ".c") {
            implementationFileList.push(this);
        }
    }
}

function printUsage() {
    console.log("Usage:\nnode ./fake.js list\nnode ./fake.js [platform name]");
}

function printPlatformDefinitions() {
    console.log("List of available platforms:");
    for (const definition of platformDefinitionList) {
        console.log(`${definition.name} (${definition.description})`);
    }
}

function splitPath(path) {
    const output = [];
    while (path !== ".") {
        output.unshift(pathUtils.basename(path));
        path = pathUtils.dirname(path);
    }
    return output;
}

function determineBuildFiles() {
    commonUtils.iterateOverDirectory(sourcePath, (path) => {
        const relativePath = pathUtils.relative(sourcePath, path);
        const componentList = splitPath(relativePath);
        if (componentList.length < 2) {
            return;
        }
        const fileCategory = componentList[0];
        if (!fileCategoryInclusionOrder.includes(fileCategory)) {
            return;
        }
        const fileName = componentList[componentList.length - 1];
        const periodIndex = fileName.lastIndexOf(".");
        if (periodIndex < 0) {
            return;
        }
        const tempExtension = fileName.substring(periodIndex, fileName.length);
        if (!sourceExtensionSet.includes(tempExtension)) {
            return;
        }
        const baseFileName = fileName.substring(0, periodIndex);
        const baseComponentList = componentList.slice(1, componentList.length);
        baseComponentList[baseComponentList.length - 1] = baseFileName;
        if (componentList[componentList.length - 2] !== "common") {
            const baseFilePath = pathUtils.join(...baseComponentList);
            if (!baseFilePathList.includes(baseFilePath)) {
                return;
            }
        }
        const intermediateFileName = componentList.join("_");
        const intermediateFilePath = pathUtils.join(intermediatePath, intermediateFileName);
        new BuildFile(fileCategory, path, intermediateFilePath);
    });
}

function convertToBaseFileName(filePath) {
    let fileName = pathUtils.basename(filePath);
    let index = fileName.lastIndexOf(".");
    if (index < 0) {
        return fileName;
    } else {
        return fileName.substring(0, index);
    }
}

function prepreprocessFiles(buildFileList) {
    for (const buildFile of buildFileList) {
        prepreprocessor.prepreprocessFile(buildFile.sourcePath, buildFile.intermediatePath);
    }
}

function substituteConstantInvocations(text) {
    for (const name in platformConstants) {
        const tempPattern = `$(${name})`;
        const tempValue = platformConstants[name];
        while (true) {
            const index = text.indexOf(tempPattern);
            if (index < 0) {
                break;
            }
            text = text.substring(0, index) + tempValue
                + text.substring(index + tempPattern.length, text.length);
        }
    }
    return text;
}

function invokeCompiler(argumentList) {
    try {
        childProcess.execFileSync(targetPlatformDefinition.compiler.name, argumentList);
    } catch(error) {
        console.log("Compiler failed. Exiting.");
        process.exit(1);
    }
}

function invokeLinker(argumentList) {
    try {
        childProcess.execFileSync(targetPlatformDefinition.linker.name, argumentList);
    } catch(error) {
        console.log("Linker failed. Exiting.");
        process.exit(1);
    }
}

if (process.argv.length !== 3) {
    printUsage();
    process.exit(1);
}

const processArgument = process.argv[2];

if (processArgument === "list") {
    printPlatformDefinitions();
    process.exit(0);
}

let targetPlatformName = processArgument;
let targetPlatformDefinition = platformDefinitionList.find((definition) => (
    targetPlatformName === definition.name
));
if (typeof targetPlatformDefinition === "undefined") {
    console.log(`Could not find platform with name "${targetPlatformName}".`);
    printPlatformDefinitions();
    process.exit(1);
}

const executablePath = pathUtils.join(buildPath, targetPlatformDefinition.executableName);
if (fs.existsSync(executablePath)) {
    fs.unlinkSync(executablePath);
}

const platformConstants = targetPlatformDefinition.constants;
platformConstants["BUILD_DIR"] = buildPath;
platformConstants["EXECUTABLE_PATH"] = executablePath;

const baseFilePathList = targetPlatformDefinition.baseFilePaths;

console.log("Base file paths for compilation:");
console.log(baseFilePathList.join("\n"));

const buildFileList = [];
const prepreprocessorFileList = [];
const headerFileList = [];
const implementationFileList = [];

determineBuildFiles();

console.log("Prepreprocessor file paths:");
console.log(prepreprocessorFileList.map((file) => file.sourcePath).join("\n"));

console.log("Header file paths:");
console.log(headerFileList.map((file) => file.sourcePath).join("\n"));

console.log("Implementation file paths:");
console.log(implementationFileList.map((file) => file.sourcePath).join("\n"));

if (fs.existsSync(intermediatePath)) {
    fs.rmdirSync(intermediatePath, {recursive: true});
}
fs.mkdirSync(intermediatePath);

console.log("Running prepreprocessor...");

const prepreprocessor = new Prepreprocessor();
for (const prepreprocessorFile of prepreprocessorFileList) {
    prepreprocessor.readDefinitionsFile(prepreprocessorFile.sourcePath);
}
prepreprocessor.initializeDefinitions();
prepreprocessFiles(headerFileList);
prepreprocessFiles(implementationFileList);
const prepreprocessorHeadersList = prepreprocessor.getHeaders();
const prepreprocessorHeadersPath = pathUtils.join(
    intermediatePath,
    "prepreprocessorHeaders.h",
);
fs.writeFileSync(prepreprocessorHeadersPath, prepreprocessorHeadersList.join("\n"));
new BuildFile("generics", null, prepreprocessorHeadersPath);

console.log("Creating constants files...");

const instructionCategoryList = JSON.parse(fs.readFileSync(bytecodeInstructionsPath, "utf8"));
const argumentAmountOffsetArray = [];
const argumentAmountArray = [];
let maximumArgAmount = 0;
for (const instructionCategory of instructionCategoryList) {
    argumentAmountOffsetArray.push(argumentAmountArray.length);
    for (const instruction of instructionCategory.instructionList) {
        const tempArgAmount = instruction.argumentList.length;
        argumentAmountArray.push(tempArgAmount);
        if (tempArgAmount > maximumArgAmount) {
            maximumArgAmount = tempArgAmount;
        }
    }
}
const argumentAmountsHeaderPath = pathUtils.join(
    intermediatePath,
    "argumentAmount.h",
);
const argumentAmountsImplementationPath = pathUtils.join(
    intermediatePath,
    "argumentAmount.c",
);
fs.writeFileSync(argumentAmountsHeaderPath, `
#define MAXIMUM_ARG_AMOUNT ${maximumArgAmount}

declareArrayConstantWithLength(argumentAmountOffsetArray, int8_t, ${argumentAmountOffsetArray.length});
declareArrayConstantWithLength(argumentAmountArray, int8_t, ${argumentAmountArray.length});
`);
fs.writeFileSync(argumentAmountsImplementationPath, `
#include "./headers.h"

declareArrayConstantWithValue(argumentAmountOffsetArray, int8_t, {${argumentAmountOffsetArray.join(",")}});
declareArrayConstantWithValue(argumentAmountArray, int8_t, {${argumentAmountArray.join(",")}});
`);
new BuildFile("constants", null, argumentAmountsHeaderPath);
new BuildFile("constants", null, argumentAmountsImplementationPath);

const platformConstantsPath = pathUtils.join(
    intermediatePath,
    "platformConstants.h",
);
const constantLineList = [];
for (const name in platformConstants) {
    const tempValue = platformConstants[name];
    constantLineList.push(`#define ${name} ${JSON.stringify(tempValue)}`);
}
fs.writeFileSync(platformConstantsPath, constantLineList.join("\n"));
new BuildFile("constants", null, platformConstantsPath);

console.log("Creating headers.h...");

headerFileList.sort((file1, file2) => file1.categoryIndex - file2.categoryIndex);
const headersLineList = headerFileList.map((file) => `#include "${file.intermediatePath}"`);
const headersPath = pathUtils.join(intermediatePath, "headers.h");
fs.writeFileSync(headersPath, headersLineList.join("\n"));

console.log("Compiling object files...");

let objectFilePathList = [];
for (const implementationFile of implementationFileList) {
    const implementationFilePath = implementationFile.intermediatePath;
    const tempBaseFileName = convertToBaseFileName(implementationFilePath);
    const objectFilePath = pathUtils.join(intermediatePath, tempBaseFileName + ".o");
    const compilerArgumentList = ["-Wall"];
    for (let flag of targetPlatformDefinition.compiler.flags) {
        flag = substituteConstantInvocations(flag);
        compilerArgumentList.push(flag);
    }
    compilerArgumentList.push("-c", implementationFilePath, "-o", objectFilePath);
    invokeCompiler(compilerArgumentList);
    objectFilePathList.push(objectFilePath);
}

console.log("Object file paths:");
console.log(objectFilePathList.join("\n"));

console.log("Linking object files...");

if (!fs.existsSync(buildPath)) {
    fs.mkdirSync(buildPath);
}
const linkerArgumentList = [];
for (let flag of targetPlatformDefinition.linker.flags) {
    flag = substituteConstantInvocations(flag);
    linkerArgumentList.push(flag);
}
for (const objectFilePath of objectFilePathList) {
    linkerArgumentList.push(objectFilePath);
}
linkerArgumentList.push("-o", executablePath);
invokeLinker(linkerArgumentList);

console.log("Finished. Executable path:");
console.log(executablePath);

for (let command of targetPlatformDefinition.commandsAfterBuild) {
    command = substituteConstantInvocations(command);
    childProcess.execSync(command, {"stdio": "inherit"});
}


