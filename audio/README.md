# EZ Audio

Native audio playback for MP3 and WAV files via pure EZ FFI (WinMM).

## Installation
```javascript
use "audio"
```

## Usage
```javascript
# Play a sound and wait for it to finish
audio.play("song.mp3", true)

# Play a sound in the background
audio.play("bgm.mp3")

# Pause and Resume
audio.pause()
wait(2000)
audio.resume()

# Stop audio
audio.stop()

# Set Volume (0 to 1000)
audio.volume(500)
```
