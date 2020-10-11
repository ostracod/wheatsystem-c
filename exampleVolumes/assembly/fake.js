
const fs = require("fs");
const pathUtils = require("path");
const childProcess = require("child_process");

const assemblerDirectoryPath = pathUtils.join(__dirname, "../../../wheatbytecode-asm");
const unixVolumePath = pathUtils.join(__dirname, "../unixVolume");
const linkedListVolumePath = pathUtils.join(__dirname, "../linkedListVolume");
const bytecodeAppNameList = ["boot", "helper"];

const fileTypeSet = {
    generic: 0,
    bytecodeApp: 1,
    systemApp: 2
};

function getAssemblyAppPath(name) {
    return pathUtils.join(__dirname, name + ".wbasm");
}

function getBytecodeAppPath(name) {
    return pathUtils.join(__dirname, name);
}

class AssemblerError extends Error {
    
    constructor(assemblerMessages) {
        super();
        this.assemblerMessages = assemblerMessages;
    }
}

function assembleBytecodeFile(name) {
    const sourcePath = getAssemblyAppPath(name);
    const destinationPath = getBytecodeAppPath(name);
    if (fs.existsSync(destinationPath)) {
        fs.unlinkSync(destinationPath);
    }
    const assemblerMessages = childProcess.execFileSync("node", [
        pathUtils.join(assemblerDirectoryPath, "/dist/assemble.js"),
        sourcePath
    ]).toString();
    if (!fs.existsSync(destinationPath)) {
        throw new AssemblerError(assemblerMessages);
    }
}

function createFileAttributes(fileType, hasAdminPerm, isGuarded) {
    return (hasAdminPerm << 3) | (isGuarded << 2) | fileType;
}

function createUnixFile(fileAttributes, name, content) {
    const destinationPath = pathUtils.join(unixVolumePath, name);
    const tempContent = Buffer.concat([Buffer.from([fileAttributes]), content]);
    fs.writeFileSync(destinationPath, tempContent);
}

function createLinkedListBuffer(fileAttributes, name, content) {
    const lastBuffer = linkedListBufferList[linkedListBufferList.length - 1];
    let tempOffset;
    if (linkedListBufferList.length > 1) {
        tempOffset = 6;
    } else {
        tempOffset = 0;
    }
    lastBuffer.writeUInt32LE(linkedListAddress, tempOffset);
    const tempHeader = Buffer.alloc(10);
    tempHeader.writeUInt8(fileAttributes, 0);
    tempHeader.writeUInt8(name.length, 1);
    tempHeader.writeUInt32LE(content.length, 2);
    tempHeader.writeUInt32LE(0, 6);
    const tempBuffer = Buffer.concat([tempHeader, Buffer.from(name), content]);
    linkedListBufferList.push(tempBuffer);
    linkedListAddress += tempBuffer.length;
}

console.log("Assembling files for example volumes...");

for (const name of bytecodeAppNameList) {
    try {
        assembleBytecodeFile(name);
    } catch(error) {
        if (error instanceof AssemblerError) {
            console.log("Assembler failed with the following output:");
            console.log("===============================================");
            console.log(error.assemblerMessages);
            process.exit(1);
        } else {
            throw error;
        }
    }
}

if (!fs.existsSync(unixVolumePath)) {
    fs.mkdirSync(unixVolumePath);
}

const tempBuffer = Buffer.alloc(4);
tempBuffer.writeUInt32LE(0, 0);
const linkedListBufferList = [tempBuffer];
let linkedListAddress = tempBuffer.length;

const bytecodeAppAttributes = createFileAttributes(fileTypeSet.bytecodeApp, false, false);
for (const name of bytecodeAppNameList) {
    const bytecodeAppPath = getBytecodeAppPath(name);
    const fileContent = fs.readFileSync(bytecodeAppPath);
    createUnixFile(bytecodeAppAttributes, name, fileContent);
    createLinkedListBuffer(bytecodeAppAttributes, name, fileContent);
}

fs.writeFileSync(linkedListVolumePath, Buffer.concat(linkedListBufferList));

console.log("Finished.");


