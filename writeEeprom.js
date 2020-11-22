
const SerialPort = require("serialport");
const fs = require("fs");

const volumePath = "./exampleVolumes/linkedListVolume";
const debugModeBufferSize = 128;

let hasReceivedResponseAck = false;
let responseBuffer = null;

const receiveResponseAck = () => new Promise((resolve, reject) => {
    const checkResponse = () => {
        if (!hasReceivedResponseAck) {
            setTimeout(checkResponse, 10);
            return;
        }
        hasReceivedResponseAck = false;
        resolve();
    };
    checkResponse();
});

const receiveResponseBuffer = () => new Promise((resolve, reject) => {
    const checkResponse = () => {
        if (!hasReceivedResponseAck || responseBuffer.length !== debugModeBufferSize) {
            setTimeout(checkResponse, 10);
            return;
        }
        const output = responseBuffer;
        hasReceivedResponseAck = false;
        responseBuffer = null;
        resolve(output);
    };
    checkResponse();
});

const createAddressBuffer = (address) => {
    const output = Buffer.alloc(3);
    output.writeUInt8(address & 0x0000FF, 0);
    output.writeUInt8((address & 0x00FF00) >> 8, 1);
    output.writeUInt8((address & 0xFF0000) >> 16, 2);
    return output;
}

const sendReadCommand = (address) => {
    serialPort.write(Buffer.concat([
        Buffer.from([33, 82]),
        createAddressBuffer(address),
    ]));
    return receiveResponseBuffer();
};

const sendWriteCommand = async (address, data) => {
    serialPort.write(Buffer.concat([
        Buffer.from([33, 87]),
        createAddressBuffer(address),
        data,
    ]));
    await receiveResponseAck();
};

const writeAndVerifyVolume = async () => {
    const volumeData = fs.readFileSync(volumePath);
    for (
        let startAddress = 0;
        startAddress < volumeData.length;
        startAddress += debugModeBufferSize
    ) {
        let tempBuffer;
        if (startAddress + debugModeBufferSize > volumeData.length) {
            tempBuffer = volumeData.slice(startAddress, volumeData.length);
            tempBuffer = Buffer.concat([
                tempBuffer,
                Buffer.alloc(debugModeBufferSize - tempBuffer.length),
            ]);
        } else {
            tempBuffer = volumeData.slice(startAddress, startAddress + debugModeBufferSize);
        }
        console.log(`Writing at address ${startAddress}...`);
        await sendWriteCommand(startAddress, tempBuffer);
    }
    for (
        let startAddress = 0;
        startAddress < volumeData.length;
        startAddress += debugModeBufferSize
    ) {
        console.log(`Reading at address ${startAddress}...`);
        const tempBuffer = await sendReadCommand(startAddress);
        let offset = 0;
        let address = startAddress;
        while (offset < tempBuffer.length && address < volumeData.length) {
            const tempValue1 = tempBuffer.readUInt8(offset);
            const tempValue2 = volumeData.readUInt8(address);
            if (tempValue1 !== tempValue2) {
                console.log(`Found data mismatch at address ${address}! (${tempValue1} != ${tempValue2})`);
                return;
            }
            offset += 1;
            address += 1;
        }
    }
    console.log("Finished writing and verifying EEPROM.");
}

console.log("Opening serial port...");

const serialPort = new SerialPort(
    "/dev/tty.usbserial-AB0JTOQV",
    {baudRate: 9600},
);

serialPort.on("open", async () => {
    console.log("Serial port is open.");
    await writeAndVerifyVolume();
    serialPort.close();
});

serialPort.on("data", data => {
    if (hasReceivedResponseAck) {
        responseBuffer = Buffer.concat([responseBuffer, data]);
    } else if (data[0] === 33) {
        hasReceivedResponseAck = true;
        responseSize = data[0];
        responseBuffer = data.slice(1, data.length);
    }
});


