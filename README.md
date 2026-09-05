


# Galaga GS

![Galaga](/assets/Galaga1.png) ![Galaga](/assets/Galaga2.png)

**An arcade-style shoot‑’em‑up for the Apple IIgs**, inspired by the original [Xgalaga](https://github.com/frank-zago/xgalaga-sdl) created by Joe Rumsey.

Xgalaga has been adapted to many systems over the years — this version brings the fast‑paced, pixel action to the Apple IIgs. Many optimizations have been done to the original code to try and trim down the amount of calculations in hopes of speeding up the code.

## GTE

Galaga GS utilizes a modified version of the [Generic Tile Engine (GTE)](https://github.com/lscharen/iigs-game-engine/tree/master) by Lucas Scharenbroich, also referred to as the iigs‑game‑engine. The original GTE implementation supported up to 16 sprites and 40 VBUFs (internal data structures used to describe sprite state).

My GTE fork extends those limits significantly:

+ 128 sprites supported
+ 56 VBUFs available

These changes were made specifically in the areas of the engine required by Galaga GS. As a result, not all GTE features are fully functional or tested in this build. Only the subsystems needed for gameplay were updated or validated.

The modified engine source code available on [github[(https://github.com/vavruska/iigs-game-engine).

## Graphics and Sounds

The sprites have been modified from the original game to fit on the Apple IIgs 320x200 screen. The sounds are almost identical to the original.

## Requirements

Galaga GS runs on an Apple IIgs running System 6.x. Galaga GS requires an accellerator. It is somewhat playable at 8Mhz but faster is more enjoyable.

### Controls

Galaga GS can be played using either the **keyboard** or the **mouse**, with the keyboard enabled by default. You can switch between control modes at any time:

- Press **`m`** to enable **mouse mode**

- Press **`k`** to return to **keyboard mode**

#### **Keyboard Mode**

- **Arrow Keys** — Move the ship **left** or **right**
- **Space Bar** — Fire torpedoes

#### **Mouse Mode**

- **Mouse Movement** — Move the ship **left** or **right**
- **Mouse Button** (or **Space Bar**) — Fire torpedoes

Mouse mode offers smoother analog movement, while keyboard mode provides classic, arcade control....although when you are pressing an arrow key and then fire a torpedo you need to re-press the arrow key to continue movement. Maybe someone will tell me how to fix this.
Both modes can be toggled instantly depending on player preference.

## The Code

The included makefile assumes that you are building using the [GoldenGate](https://juiced.gs/store/golden-gate/) compatibility layer by Kelvin Sherlock under Linux.
  
## Special Thanks

Thanks go out to Lucas Scharenbroich. If he had not developed and released source to GTE I never would have been able to start this version of Galaga. 
