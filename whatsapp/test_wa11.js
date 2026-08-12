const { makeWASocket, useMultiFileAuthState } = require("@whiskeysockets/baileys");
const fs = require("fs");
const ws = require("ws");

// We will monkey patch ws to intercept the sent data
const originalSend = ws.prototype.send;
ws.prototype.send = function(data, options, cb) {
    console.log("SENDING:", Buffer.from(data).toString("hex"));
    return originalSend.call(this, data, options, cb);
};

const originalOnMessage = ws.prototype.on;
ws.prototype.on = function(event, listener) {
    if (event === "message") {
        const originalListener = listener;
        listener = function(data) {
            console.log("RECEIVED:", Buffer.from(data).toString("hex"));
            return originalListener.apply(this, arguments);
        };
    }
    return originalOnMessage.call(this, event, listener);
};

async function run() {
    const { state, saveCreds } = await useMultiFileAuthState("auth_info_baileys");
    const sock = makeWASocket({
        auth: state,
        printQRInTerminal: false,
    });
    sock.ev.on("creds.update", saveCreds);
    
    setTimeout(() => process.exit(0), 3000);
}
run();

