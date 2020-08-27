
const fs = require("fs");
const pathUtils = require("path");
const childProcess = require("child_process");

const assemblerDirectoryPath = pathUtils.join(__dirname, "../../../wheatbytecode-asm");
const unixVolumePath = pathUtils.join(__dirname, "../unixVolume");
const bytecodeAppNameList = ["boot"];

const fileTypeSet = {
    generic: 0,
    bytecodeApp: 1,
    systemApp: 2
};

function assembleBytecodeFile(sourcePath) {
    childProcess.execFileSync("node", [
        pathUtils.join(assemblerDirectoryPath, "/dist/assemble.js"),
        sourcePath
    ]);
}

function createFileAttributes(fileType, hasAdminPerm, isGuarded) {
    return (hasAdminPerm << 3) | (isGuarded << 2) | fileType;
}

function addUnixFileHeader(sourcePath, destinationPath, fileAttributes) {
    let tempContent = fs.readFileSync(sourcePath);
    tempContent = Buffer.concat([
        Buffer.from([fileAttributes]),
        tempContent
    ]);
    fs.writeFileSync(destinationPath, tempContent);
}

console.log("Assembling files for example volumes...");

for (const name of bytecodeAppNameList) {
    assembleBytecodeFile(pathUtils.join(__dirname, name + ".wbasm"));
}

if (!fs.existsSync(unixVolumePath)) {
    fs.mkdirSync(unixVolumePath);
}

const bytecodeAppAttributes = createFileAttributes(fileTypeSet.bytecodeApp, false, false);
for (const name of bytecodeAppNameList) {
    const bytecodeAppPath = pathUtils.join(__dirname, name);
    addUnixFileHeader(
        bytecodeAppPath,
        pathUtils.join(unixVolumePath, name),
        bytecodeAppAttributes
    );
}

console.log("Finished.");


