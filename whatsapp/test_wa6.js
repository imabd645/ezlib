const { makeWASocket, useMultiFileAuthState } = require("@whiskeysockets/baileys");
async function run() {
    const { state, saveCreds } = await useMultiFileAuthState("auth_info_baileys");
    const sock = makeWASocket({
        auth: state,
        printQRInTerminal: true,
        logger: require("pino")({ level: "trace" })
    });
    sock.ev.on("creds.update", saveCreds);
}
run();

