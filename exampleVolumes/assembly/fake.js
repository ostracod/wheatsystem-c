
const fs = require("fs");
const pathUtils = require("path");
const childProcess = require("child_process");

const assemblerDirectoryPath = pathUtils.join(__dirname, "../../../wheatbytecode-asm");
const volumePath = pathUtils.join(__dirname, "../unixVolume");

const fileTypeSet = {
    generic: 0,
    bytecodeApplication: 1,
    systemApplication: 2
};

function assembleBytecodeFile(sourcePath) {
    childProcess.execFileSync("node", [
        pathUtils.join(assemblerDirectoryPath, "/dist/assemble.js"),
        sourcePath
    ]);
}

// The file header consists of a single byte:
// Header = 0x20 | (hasAdminPerm << 3) | (isGuarded << 2) | fileType
function addFileHeader(sourcePath, destinationPath, fileType) {
    let tempContent = fs.readFileSync(sourcePath);
    tempContent = Buffer.concat([
        Buffer.from([0x20 | fileType]),
        tempContent
    ]);
    fs.writeFileSync(destinationPath, tempContent);
}

console.log("Assembling files for example volumes...");

assembleBytecodeFile(pathUtils.join(__dirname, "boot.wbasm"));

if (!fs.existsSync(volumePath)) {
    fs.mkdirSync(volumePath);
}

addFileHeader(
    pathUtils.join(__dirname, "boot"),
    pathUtils.join(volumePath, "boot"),
    fileTypeSet.bytecodeApplication
);

console.log("Finished.");


