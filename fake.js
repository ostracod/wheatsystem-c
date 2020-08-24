
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

function invokeGcc(argumentList) {
    try {
        childProcess.execFileSync("gcc", argumentList);
    } catch(error) {
        console.log("Compilation failed. Exiting.");
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

const executablePath = pathUtils.join(buildPath, targetPlatformName + "WheatSystem");
if (fs.existsSync(executablePath)) {
    fs.unlinkSync(executablePath);
}

const baseFilePathList = [pathUtils.join(platformsPath, targetPlatformDefinition.name)];
for (const baseFilePath of targetPlatformDefinition.baseFilePaths) {
    baseFilePathList.push(pathUtils.join(sourcePath, baseFilePath));
}
const commonBaseFileNames = getBaseFileNamesInDirectory(commonPath);
for (const baseFileName of commonBaseFileNames) {
    baseFilePathList.push(pathUtils.join(commonPath, baseFileName));
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

declareArrayConstantWithSize(argumentAmountOffsetArray, ${argumentAmountOffsetArray.length});
declareArrayConstantWithSize(argumentAmountArray, ${argumentAmountArray.length});
`);
fs.writeFileSync(argumentAmountsImplementationPath, `
#include "./headers.h"

declareArrayConstantWithValue(argumentAmountOffsetArray, {${argumentAmountOffsetArray.join(",")}});
declareArrayConstantWithValue(argumentAmountArray, {${argumentAmountArray.join(",")}});
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
    invokeGcc([
        "-Wall",
        "-c",
        implementationfilePath,
        "-o",
        objectFilePath
    ]);
    objectFilePathList.push(objectFilePath);
}

console.log("Object file paths:");
console.log(objectFilePathList.join("\n"));

console.log("Linking object files...");

if (!fs.existsSync(buildPath)) {
    fs.mkdirSync(buildPath);
}
const linkArgumentList = objectFilePathList.slice();
linkArgumentList.push("-o", executablePath);
invokeGcc(linkArgumentList);

console.log("Finished. Executable path:");
console.log(executablePath);


