# ezaudio

The `ezaudio` package provides native WinMM FFI audio playback for MP3 and WAV files on Windows.

## Advanced Usage (Object-Oriented)

The new `AudioTrack` class allows you to play **multiple sounds simultaneously** (e.g., background music + sound effects), loop audio seamlessly, and query playback data!

```ez
use "ezaudio"

bgm = AudioTrack("music.mp3")
sfx = AudioTrack("jump.wav")

# Play background music on repeat!
bgm.play(true)

# Play a sound effect once
sfx.play()

# Check duration and position
out "Total length: " + str(bgm.getDuration()) + " ms"
out "Current time: " + str(bgm.getPosition()) + " ms"

# Jump to exactly 10 seconds into the song
bgm.seek(10000)

# Volume control (0 - 1000)
bgm.volume(500)

# Stop or pause
bgm.pause()
bgm.resume()

# Always close tracks when done to free memory!
bgm.close()
sfx.close()
```

## Legacy Usage (Functional)

For backwards compatibility and simple single-track scripts, the old functional API is still available:

```ez
use "ezaudio"

# Play a track (if another is playing via the legacy API, it stops automatically)
play("sound.wav")

# Optionally wait for it to finish before continuing the script
play("sound.wav", true)

pause()
resume()
stop()
volume(1000)
```
