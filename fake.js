
const fs = require("fs");
const pathUtils = require("path");
const childProcess = require("child_process");
const Prepreprocessor = require("./prepreprocessor").Prepreprocessor;

const sourcePath = pathUtils.join(__dirname, "src");
const commonPath = pathUtils.join(sourcePath, "common");
const platformsPath = pathUtils.join(sourcePath, "platforms");
const intermediatePath = pathUtils.join(__dirname, "intermediate");
const buildPath = pathUtils.join(__dirname, "build");
const bytecodeInstructionsPath = pathUtils.join(__dirname, "..", "wheatsystem-spec", "bytecodeInstructions.json");

const platformDefinitionsPath = pathUtils.join(__dirname, "platformDefinitions.json");
const platformDefinitionList = JSON.parse(fs.readFileSync(platformDefinitionsPath), "utf8");

function printUsage() {
    console.log("Usage:\nnode ./fake.js list\nnode ./fake.js [platform name]");
}

function printPlatformDefinitions() {
    console.log("List of available platforms:");
    for (const definition of platformDefinitionList) {
        console.log(`${definition.name} (${definition.description})`);
    }
}

function getBaseFileNamesInDirectory(directoryPath) {
    const fileNameList = fs.readdirSync(directoryPath);
    const baseFileNameSet = new Set();
    for (const fileName of fileNameList) {
        if (fileName.length < 2) {
            continue;
        }
        const tempSuffix = fileName.substring(fileName.length - 2, fileName.length);
        if (tempSuffix === ".h" || tempSuffix === ".c") {
            const baseFileName = fileName.substring(0, fileName.length - 2);
            baseFileNameSet.add(baseFileName);
        }
    }
    return Array.from(baseFileNameSet);
}

function getFilePathsWithExtension(extension) {
    const output = [];
    for (const baseFilePath of baseFilePathList) {
        const filePath = baseFilePath + extension;
        if (fs.existsSync(filePath)) {
            output.push(filePath);
        }
    }
    return output;
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

function findFirstCommonFile(pathList) {
    for (let index = 0; index < pathList.length; index++) {
        const tempPath = pathList[index];
        const directoryName = pathUtils.basename(pathUtils.dirname(tempPath));
        if (directoryName === "common") {
            return index;
        }
    }
    return -1;
}

function prepreprocessFiles(filePathList) {
    const output = [];
    for (const sourceFilePath of filePathList) {
        const fileName = pathUtils.basename(sourceFilePath);
        const destinationFilePath = pathUtils.join(intermediatePath, fileName);
        prepreprocessor.prepreprocessFile(sourceFilePath, destinationFilePath);
        output.push(destinationFilePath);
    }
    return output;
}

function substituteConstantInvocations(text) {
    for (const name in targetPlatformConstants) {
        const tempPattern = `$(${name})`;
        const tempValue = targetPlatformDefinition.constants[name];
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
let targetPlatformDefinition = null;
for (const definition of platformDefinitionList) {
    if (targetPlatformName === definition.name) {
        targetPlatformDefinition = definition;
        break;
    }
}
if (targetPlatformDefinition === null) {
    console.log(`Could not find platform with name "${targetPlatformName}".`);
    printPlatformDefinitions();
    process.exit(1);
}

const executablePath = pathUtils.join(buildPath, targetPlatformDefinition.executableName);
if (fs.existsSync(executablePath)) {
    fs.unlinkSync(executablePath);
}

const targetPlatformConstants = targetPlatformDefinition.constants;
targetPlatformConstants["BUILD_DIR"] = buildPath;
targetPlatformConstants["EXECUTABLE_PATH"] = executablePath;

const baseFilePathList = [pathUtils.join(platformsPath, targetPlatformDefinition.name)];
const commonBaseFileNames = getBaseFileNamesInDirectory(commonPath);
for (const baseFileName of commonBaseFileNames) {
    baseFilePathList.push(pathUtils.join(commonPath, baseFileName));
}
for (const baseFilePath of targetPlatformDefinition.baseFilePaths) {
    baseFilePathList.push(pathUtils.join(sourcePath, baseFilePath));
}

console.log("Base file paths for compilation:");
console.log(baseFilePathList.join("\n"));

let headerFilePathList = getFilePathsWithExtension(".h");
let implementationFilePathList = getFilePathsWithExtension(".c");

console.log("Header file paths:");
console.log(headerFilePathList.join("\n"));

console.log("Implementation file paths:");
console.log(implementationFilePathList.join("\n"));

const firstCommonHeaderIndex = findFirstCommonFile(headerFilePathList);
const firstCommonImplementationIndex = findFirstCommonFile(implementationFilePathList);

if (fs.existsSync(intermediatePath)) {
    fs.rmdirSync(intermediatePath, {recursive: true});
}
fs.mkdirSync(intermediatePath);

console.log("Running prepreprocessor...");

const prepreprocessorDefinitionsPath = pathUtils.join(sourcePath, "prepreprocessorDefinitions.pppd");
const prepreprocessor = new Prepreprocessor(prepreprocessorDefinitionsPath);
headerFilePathList = prepreprocessFiles(headerFilePathList);
implementationFilePathList = prepreprocessFiles(implementationFilePathList);

console.log("Creating argument amount array...");

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
argumentAmountsHeaderPath = pathUtils.join(intermediatePath, "argumentAmount.h");
argumentAmountsImplementationPath = pathUtils.join(intermediatePath, "argumentAmount.c");
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
headerFilePathList.splice(firstCommonHeaderIndex, 0, argumentAmountsHeaderPath);
implementationFilePathList.splice(firstCommonImplementationIndex, 0, argumentAmountsImplementationPath);

console.log("Creating headers.h...");

const headersLineList = headerFilePathList.map(filePath => `#include "${filePath}"`);
const headersPath = pathUtils.join(intermediatePath, "headers.h");
fs.writeFileSync(headersPath, headersLineList.join("\n"));

console.log("Compiling object files...");

let objectFilePathList = [];
for (const implementationfilePath of implementationFilePathList) {
    const tempBaseFileName = convertToBaseFileName(implementationfilePath);
    const objectFilePath = pathUtils.join(intermediatePath, tempBaseFileName + ".o");
    const compilerArgumentList = ["-Wall"];
    for (let flag of targetPlatformDefinition.compiler.flags) {
        flag = substituteConstantInvocations(flag);
        compilerArgumentList.push(flag);
    }
    compilerArgumentList.push("-c", implementationfilePath, "-o", objectFilePath);
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


