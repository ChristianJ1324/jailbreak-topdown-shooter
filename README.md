# Jailbreak — Top-Down 2D Shooter (C++ / SFML)

A top-down 2D action shooter built in **C++** and **SFML**, featuring bullet-time slow motion, multi-charge dashing, enemy AI, checkpoints, save/load progression, and an in-game debug overlay.

## Features
- **Combat & Movement:** Smooth WASD movement, mouse aiming, pistol firing, and a 3-charge **Dash** (refreshed on enemy kills).
- **Bullet Time:** Hold right-click to slow down time using the bullet-time meter.
- **Level Progression & Checkpoints:** Clear all enemies in a prison block to advance to the next level; triggered checkpoints allow resuming after death.
- **Save / Load System:** Persists current level, player health, position, checkpoint, dash charges, and bullet-time meter.
- **Debug Overlay (`F1`):** Toggle invincibility, restore health, adjust speed, skip/reload levels, visualize collision boxes, and freeze enemy AI.

## Controls
| Input | Action |
| :--- | :--- |
| `WASD` / Arrow Keys | Move |
| `Space` | Dash |
| `Mouse` | Aim |
| `Left Click` | Fire pistol |
| `Right Click` (Hold) | Bullet time |
| `Esc` / `Tab` | Pause / Resume |
| `F1` | Toggle debug overlay |

## Building & Running
Open `ChristianJamieson_Assessment3.sln` in **Visual Studio** (C++ / MSVC) and build & run (`Win32` / `x64` Release or Debug).
