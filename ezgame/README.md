# ezgame — GUI Game Engine for EZ

> **Version:** 1.0  
> **Import:** `use "ezgame"`  
> **File:** `E:\ezlib\ezgame\main.ez`  
> **Requires:** `winmm.dll`, `user32.dll`, `gdi32.dll`, `msimg32.dll`

---

## Overview

`ezgame` is a real-time game engine for EZ built on native GDI and Windows Multimedia APIs. It provides:

- **Game window creation** via EZ GUI built-ins
- **Game loop** with configurable FPS using a timer
- **Keyboard input** polling
- **Drawing primitives** (rect, circle, text)
- **Audio** (play, loop, stop via MCI)
- **Image loading and drawing** with transparency (BMP via GDI)

---

## Quick Start

```ez
use "ezgame"

game = GUIGame("My Game", 800, 600)
game.setFPS(60)

x = 100
y = 100
speed = 5

game.onUpdateCall(|| {
    when game.isKeyDown(KEYS["RIGHT"]) { x = x + speed }
    when game.isKeyDown(KEYS["LEFT"])  { x = x - speed }
    when game.isKeyDown(KEYS["UP"])    { y = y - speed }
    when game.isKeyDown(KEYS["DOWN"])  { y = y + speed }
})

game.onDrawCall(|| {
    game.clear(30, 30, 30)           # Dark background
    game.drawRect(x, y, 50, 50, 255, 100, 0)  # Orange square
    game.drawText("EZ Game", 10, 10, 16, 255, 255, 255)  # White text
})

game.run()
```

---

## KEYS Dictionary

Pre-defined virtual key codes for common keys.

```ez
use "ezgame"

out KEYS["LEFT"]   # → 37
out KEYS["UP"]     # → 38
out KEYS["RIGHT"]  # → 39
out KEYS["DOWN"]   # → 40
out KEYS["SPACE"]  # → 32
out KEYS["ESC"]    # → 27
out KEYS["ENTER"]  # → 13
out KEYS["W"]      # → 87
out KEYS["A"]      # → 65
out KEYS["S"]      # → 83
out KEYS["D"]      # → 68
```

For other keys, use their Windows Virtual Key Code directly (e.g. `70` for F).

---

## Model: `GUIGame`

### `GUIGame(title, width, height)`

Creates a game window.

**Parameters:**
- `title` — Window title string.
- `width` — Window width in pixels.
- `height` — Window height in pixels.

```ez
use "ezgame"

game = GUIGame("Space Shooter", 800, 600)
```

---

### `game.setFPS(fps)`

Sets the target frames per second. The timer interval is `1000 / fps` milliseconds.

```ez
game.setFPS(30)   # 30 FPS (16ms per frame) — typical for platformers
game.setFPS(60)   # 60 FPS (16ms per frame) — smooth
game.setFPS(120)  # 120 FPS — high refresh rate
```

---

### `game.onUpdateCall(callback)`

Registers the update callback. Called every frame to update game state (physics, input, AI).

```ez
game.onUpdateCall(|| {
    # Update game logic
    player.x = player.x + player.vx
    player.y = player.y + player.vy
})
```

---

### `game.onDrawCall(callback)`

Registers the draw callback. Called every frame to render.

```ez
game.onDrawCall(|| {
    game.clear(0, 0, 0)                              # Black background
    game.drawCircle(player.x, player.y, 20, 0, 200, 255)  # Blue circle
})
```

---

### `game.run()`

Starts the game loop. This:
1. Sets up the frame timer (`gui_set_timer`)
2. Shows the window (`gui_show`)
3. Enters the Windows message loop (`gui_run`) — blocks until the window is closed.

```ez
game.run()
# → Game loop starts here
```

---

### `game.stop()`

Stops the game loop and kills the timer. Call from within a callback when the game should end.

```ez
game.onUpdateCall(|| {
    when game.isKeyDown(KEYS["ESC"]) {
        game.stop()
    }
})
```

---

## Drawing Methods

All drawing methods use the game's window handle internally.

### `game.clear(r, g, b)`

Clears the entire window with a solid color. Call this at the start of each `onDrawCall` to prevent flickering from previous frames.

```ez
game.clear(0, 0, 0)       # Black
game.clear(30, 30, 46)    # Dark navy
game.clear(135, 206, 235) # Sky blue
```

---

### `game.drawRect(x, y, w, h, r, g, b)`

Draws a filled rectangle.

**Parameters:**
- `x, y` — Top-left corner position (pixels from top-left of window).
- `w, h` — Width and height in pixels.
- `r, g, b` — RGB color (0–255 each).

```ez
game.drawRect(100, 100, 50, 30, 255, 0, 0)  # Red rectangle
game.drawRect(200, 150, 80, 80, 0, 255, 0)  # Green square
```

---

### `game.drawCircle(x, y, radius, r, g, b)`

Draws a filled circle.

```ez
game.drawCircle(400, 300, 50, 255, 255, 0)   # Yellow circle (center)
game.drawCircle(100, 100, 20, 0, 0, 255)     # Blue circle
```

---

### `game.drawText(text, x, y, size, r, g, b)`

Draws text.

- `x, y` — Position of top-left of text.
- `size` — Font size in pixels.
- `r, g, b` — Text color.

```ez
game.drawText("Score: " + str(score), 10, 10, 18, 255, 255, 255)  # White score
game.drawText("GAME OVER", 300, 280, 32, 255, 50, 50)              # Red big text
```

---

## Input

### `game.isKeyDown(keyCode)` → `boolean`

Returns `true` if the given key is currently pressed.

Uses `gui_is_key_down(key)` which wraps the Win32 `GetAsyncKeyState`.

```ez
game.onUpdateCall(|| {
    when game.isKeyDown(KEYS["LEFT"])  { player.x = player.x - 5 }
    when game.isKeyDown(KEYS["RIGHT"]) { player.x = player.x + 5 }
    when game.isKeyDown(KEYS["UP"])    { player.y = player.y - 5 }
    when game.isKeyDown(KEYS["DOWN"])  { player.y = player.y + 5 }
    when game.isKeyDown(KEYS["SPACE"]) { shoot() }
    when game.isKeyDown(KEYS["ESC"])   { game.stop() }
})
```

---

## Utility

### `game.getCenter()` → `array`

Returns `[width/2, height/2]` — the center coordinates of the window.

```ez
center = game.getCenter()
cx = center[0]   # → 400 (for 800-wide window)
cy = center[1]   # → 300 (for 600-tall window)
```

---

## Audio Methods

### `game.playSound(path)`
Plays a sound file once. Uses MCI (Media Control Interface).

```ez
game.playSound("sounds\\shoot.wav")
game.playSound("C:\\Games\\MyGame\\sounds\\explosion.wav")
```

### `game.loopSound(path)`
Plays a sound file in a continuous loop.

```ez
game.loopSound("sounds\\background_music.wav")
```

### `game.stopSound(path)`
Stops a playing sound.

```ez
game.stopSound("sounds\\background_music.wav")
```

**Supported formats:** WAV files are most reliable. MP3 support depends on Windows codecs.

---

## Image Methods

### `game.loadImage(path)` → `handle`

Loads a BMP image from file. Returns a native bitmap handle (integer pointer).

```ez
use "ezgame"

game = GUIGame("Image Test", 800, 600)
imgHandle = game.loadImage("sprites\\player.bmp")
```

---

### `game.drawImage(imgHandle, x, y, w, h, srcW, srcH, tR, tG, tB)`

Draws a BMP image with color-key transparency.

**Parameters:**
- `imgHandle` — Handle returned from `loadImage()`.
- `x, y` — Destination position on screen.
- `w, h` — Destination draw size.
- `srcW, srcH` — Source bitmap dimensions.
- `tR, tG, tB` — Transparent color (pixels of this color become transparent).

```ez
game.onDrawCall(|| {
    game.clear(0, 0, 0)
    
    # Draw player sprite (magenta = transparent)
    game.drawImage(playerImg, player.x, player.y, 48, 48, 48, 48, 255, 0, 255)
})
```

**Notes:**
- Internally uses `TransparentBlt` from `msimg32.dll`.
- Images must be Windows BMP format.
- The transparent color is specified as an RGB value.

---

## Factory Function

### `createGame(title, w, h)` → `GUIGame`

Convenience function.

```ez
use "ezgame"

game = createGame("My Game", 800, 600)
```

---

## Edge Cases & Important Notes

### Coordinate System
The game window uses top-left as `(0, 0)`. X increases to the right, Y increases downward. This matches screen coordinates (unlike PDF which is bottom-up).

### Frame Rate Accuracy
The timer is based on Windows `WM_TIMER` messages. Timer resolution is typically 15ms minimum on most Windows systems. `setFPS(120)` may not actually achieve 120 FPS reliably.

### `clear()` Each Frame
Always call `game.clear()` at the beginning of `onDrawCall`. Without it, previous frames accumulate and create trails or artifacts.

### Audio File Paths
Sound file paths must use backslashes or full paths. Relative paths are relative to the executable's working directory.

### Image Memory
Loaded images (`loadImage()`) return HBITMAP handles. These are not automatically freed. For many different images, accumulated handles may consume memory. Currently there is no `freeImage()` method.

### `getCenter()` Returns Array
`getCenter()` returns `[width/2, height/2]`. Access with index:
```ez
c = game.getCenter()
cx = c[0]
cy = c[1]
```

### findWindow in drawImage
`drawImage()` calls `FindWindowA(0, self.title)` to get the HWND each call. If multiple windows have the same title, it may draw to the wrong window. Use unique titles per game instance.

---

## Full Example: Pong

```ez
use "ezgame"

W = 800
H = 600
PADDLE_W = 15
PADDLE_H = 80
BALL_SIZE = 15

ball = {"x": 400, "y": 300, "vx": 4, "vy": 3}
p1 = {"x": 30, "y": 250}
p2 = {"x": W - 45, "y": 250}
score = {"p1": 0, "p2": 0}

game = GUIGame("Pong", W, H)
game.setFPS(60)

game.onUpdateCall(|| {
    # Player 1 controls
    when game.isKeyDown(KEYS["W"]) and p1.y > 0 { p1.y = p1.y - 6 }
    when game.isKeyDown(KEYS["S"]) and p1.y < H - PADDLE_H { p1.y = p1.y + 6 }
    
    # Player 2 controls
    when game.isKeyDown(KEYS["UP"]) and p2.y > 0 { p2.y = p2.y - 6 }
    when game.isKeyDown(KEYS["DOWN"]) and p2.y < H - PADDLE_H { p2.y = p2.y + 6 }
    
    # Ball movement
    ball.x = ball.x + ball.vx
    ball.y = ball.y + ball.vy
    
    # Wall bounce (top/bottom)
    when ball.y < 0 or ball.y > H - BALL_SIZE { ball.vy = -ball.vy }
    
    # Paddle collision P1
    when ball.x < p1.x + PADDLE_W and ball.y > p1.y and ball.y < p1.y + PADDLE_H {
        ball.vx = abs(ball.vx)
    }
    
    # Paddle collision P2
    when ball.x + BALL_SIZE > p2.x and ball.y > p2.y and ball.y < p2.y + PADDLE_H {
        ball.vx = -abs(ball.vx)
    }
    
    # Scoring
    when ball.x < 0 {
        score.p2 = score.p2 + 1
        ball.x = W / 2   ball.y = H / 2
        ball.vx = 4
    }
    when ball.x > W {
        score.p1 = score.p1 + 1
        ball.x = W / 2   ball.y = H / 2
        ball.vx = -4
    }
    
    when game.isKeyDown(KEYS["ESC"]) { game.stop() }
})

game.onDrawCall(|| {
    game.clear(20, 20, 20)
    
    # Center line
    i = 0
    while i < H {
        game.drawRect(W/2 - 2, i, 4, 15, 80, 80, 80)
        i = i + 25
    }
    
    # Paddles
    game.drawRect(p1.x, p1.y, PADDLE_W, PADDLE_H, 100, 200, 255)
    game.drawRect(p2.x, p2.y, PADDLE_W, PADDLE_H, 255, 100, 100)
    
    # Ball
    game.drawRect(ball.x, ball.y, BALL_SIZE, BALL_SIZE, 255, 255, 255)
    
    # Scores
    game.drawText(str(score.p1), W/2 - 60, 20, 28, 200, 200, 200)
    game.drawText(str(score.p2), W/2 + 30, 20, 28, 200, 200, 200)
})

game.run()
```

---

## Full Example: Simple Platformer

```ez
use "ezgame"

W = 800
H = 500
GRAVITY = 0.5
JUMP_SPEED = -12

player = {"x": 100, "y": 300, "vx": 0, "vy": 0, "onGround": false}
platforms = [
    {"x": 0, "y": 450, "w": 800, "h": 50},   # Ground
    {"x": 200, "y": 350, "w": 150, "h": 20},
    {"x": 450, "y": 280, "w": 150, "h": 20},
    {"x": 650, "y": 200, "w": 120, "h": 20}
]

game = GUIGame("Platformer", W, H)
game.setFPS(60)

game.onUpdateCall(|| {
    # Horizontal movement
    player.vx = 0
    when game.isKeyDown(KEYS["LEFT"])  { player.vx = -5 }
    when game.isKeyDown(KEYS["RIGHT"]) { player.vx = 5 }
    
    # Jump
    when game.isKeyDown(KEYS["SPACE"]) and player.onGround {
        player.vy = JUMP_SPEED
        player.onGround = false
    }
    
    # Apply gravity
    player.vy = player.vy + GRAVITY
    
    # Move
    player.x = player.x + player.vx
    player.y = player.y + player.vy
    player.onGround = false
    
    # Platform collision
    get plat in platforms {
        when player.x + 30 > plat.x and player.x < plat.x + plat.w {
            when player.y + 40 > plat.y and player.y + 40 - player.vy <= plat.y {
                player.y = plat.y - 40
                player.vy = 0
                player.onGround = true
            }
        }
    }
    
    # Boundaries
    when player.x < 0 { player.x = 0 }
    when player.x > W - 30 { player.x = W - 30 }
    
    when game.isKeyDown(KEYS["ESC"]) { game.stop() }
})

game.onDrawCall(|| {
    game.clear(50, 120, 200)   # Sky
    
    # Platforms
    get plat in platforms {
        game.drawRect(plat.x, plat.y, plat.w, plat.h, 80, 150, 60)
    }
    
    # Player
    game.drawRect(player.x, player.y, 30, 40, 255, 200, 50)   # Yellow player
    
    game.drawText("Use ARROWS + SPACE", 10, 10, 14, 255, 255, 255)
})

game.run()
```

---

*Documentation generated from `E:\ezlib\ezgame\main.ez` — EZ Game Engine Library*
