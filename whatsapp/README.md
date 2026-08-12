# whatsapp

A WhatsApp Web client for EZ — the Noise Protocol handshake and binary node
format, in the style of Baileys.

```ez
use "whatsapp"

wa = WAClient()
wa.connect()                    // opens the socket, runs the Noise handshake
wa.waitForQR()                  // prints a QR code, blocks until you scan it

wa.on("message", |msg| { out msg["from"] + ": " + msg["text"] })
wa.sendText("1234567890@s.whatsapp.net", "Hello!")

wa.loop()                       // stay connected
```

## Install

```
ez install whatsapp
```

Depends on `crypto`.

## What this actually is

WhatsApp Web is not a documented API. This implements its protocol from the
outside, the way Baileys does:

| module | |
|---|---|
| `wa_crypto.ez` | Curve25519 ECDH, AES-256-GCM, HMAC-SHA256, HKDF, via `bcrypt.dll` |
| `wa_noise.ez` | the Noise Protocol XX handshake state machine |
| `wa_ws.ez` | the WebSocket connection, via `winhttp.dll` |
| `wa_proto.ez` | the binary node encoder/decoder, including the token tables |
| `wa_qr.ez` | QR rendering as terminal art |
| `wa_store.ez` | session credentials on disk |
| `main.ez` | the `WAClient` facade |

Read that as a warning as much as a description. An undocumented protocol
changes without notice, and when it does this stops working until the format is
worked out again. Automating a WhatsApp account can also get the number banned —
that is WhatsApp's call, not this library's. Use it on a number you can afford
to lose.

## Connecting

```ez
wa = WAClient()
wa.connect()
wa.waitForQR()
```

`waitForQR` prints a QR code to the terminal and blocks until the phone scans
it. After a successful pairing the session is written to `wa_session.json` in
the working directory, and later runs reuse it rather than asking again.

**That file holds a private key, in plaintext.** Keep it out of version
control, out of shared directories, and out of any package you publish.

```ez
wa.isConnected()
wa.status()
wa.disconnect()
```

## Sending

```ez
wa.sendText(jid, "Hello!")
wa.sendImage(jid, imageBytes, "a caption")
wa.sendTyping(jid)
wa.markRead(jid, messageId)
```

A `jid` is WhatsApp's address: `1234567890@s.whatsapp.net` for a person,
`…@g.us` for a group. It is the number in full international form with no `+`.

## Receiving

```ez
wa.on("message", |msg| { … })
wa.on("connected", |info| { … })
wa.on("disconnected", |reason| { … })
```

Then either give the client the thread:

```ez
wa.loop()
```

or drive it from your own:

```ez
while running {
    wa.poll()
    doOtherWork()
}
```

## Message types

```ez
WA_MSG_TEXT       WA_MSG_IMAGE      WA_MSG_DOCUMENT
WA_MSG_AUDIO      WA_MSG_VIDEO      WA_MSG_STICKER
```

## The lower layers

The protocol pieces are exported, so the primitives are usable on their own —
`wac_generateKeyPair`, `wac_ecdh`, `wac_hkdf`, `wac_aesGcmEncrypt`,
`wac_aesGcmDecrypt`, `wac_hmacSha256`, `wac_sha256`, and the varint and
protobuf helpers in `wa_proto.ez`.

## Status

Unlike most of this library, the WhatsApp client is not something that can be
verified by a test suite — it needs a real account, a real phone and a live
handshake, and its counterpart can change at any time. Treat it as working code
to build on rather than a settled interface, and check the behaviour you depend
on yourself.

## License

MIT
