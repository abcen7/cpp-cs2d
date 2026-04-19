# Counter-Strike 2D Simplified (CS2DS)

## Project Overview

CS2DS is a simplified clone of [CS2D](https://www.cs2d.com/) — a 2D top-down arcade shooter built in **C++20** with **FLTK** graphics library, following the **MVC** architecture pattern. The game features a singleplayer Deathmatch mode where the player fights AI-controlled bots.

**Key constraints (university requirements — must be followed strictly):**

- Language: C++20
- Graphics: FLTK 1.3.x (no other GUI/rendering libraries)
- Architecture: strict MVC separation into directories
- Smart pointers: `std::unique_ptr` / `std::shared_ptr` instead of raw `new`/`delete`
- Multithreading: 2 threads (rendering in main thread, game logic in a separate thread)
- No global variables (except `main`)
- All class fields must be `private`
- All four OOP relationships required: inheritance, composition, aggregation, association
- FLTK types/classes allowed **only** in View layer
- `void*` allowed **only** in FLTK static callback wrappers
- Callback registration: only the class whose method is called may set the callback
- After game over the program returns to the main menu **without recreating the window**

---

## Tech Stack

| Component | Technology |
|---|---|
| Language | C++20 (ISO/IEC 14882:2020) |
| GUI/Rendering | FLTK 1.4.x |
| Build system | CMake 3.20+ |
| Architecture | Model-View-Controller |
| Memory management | `std::unique_ptr`, `std::shared_ptr`, `std::weak_ptr` |
| Threading | `std::thread`, `std::mutex`, `std::atomic`, `std::condition_variable` |
| Platform | Windows (MSVC/MinGW) and Linux (GCC) |
| VCS | Git (GitVerse) |

---

## Project Structure

```
CS2DS/
├── CMakeLists.txt
├── README.md
├── assets/
│   ├── maps/
│   │   └── de_simple.map
│   └── textures/                # optional
├── src/
│   ├── main.cpp
│   ├── model/
│   │   ├── game_object.h / .cpp
│   │   ├── character.h / .cpp
│   │   ├── player.h / .cpp
│   │   ├── bot.h / .cpp
│   │   ├── bot_state_machine.h / .cpp
│   │   ├── weapon.h / .cpp
│   │   ├── pistol.h / .cpp
│   │   ├── rifle.h / .cpp
│   │   ├── shotgun.h / .cpp
│   │   ├── bullet.h / .cpp
│   │   ├── bonus.h / .cpp
│   │   ├── health_pack.h / .cpp
│   │   ├── ammo_pack.h / .cpp
│   │   ├── armor_pack.h / .cpp
│   │   ├── game_map.h / .cpp
│   │   ├── tile.h / .cpp
│   │   ├── waypoint.h / .cpp
│   │   ├── collision_manager.h / .cpp
│   │   └── game_state.h / .cpp
│   ├── view/
│   │   ├── main_window.h / .cpp
│   │   ├── menu_view.h / .cpp
│   │   ├── game_view.h / .cpp
│   │   ├── hud_view.h / .cpp
│   │   ├── about_view.h / .cpp
│   │   └── game_over_view.h / .cpp
│   └── controller/
│       ├── game_controller.h / .cpp
│       ├── input_controller.h / .cpp
│       └── game_loop_controller.h / .cpp
└── tests/                       # optional
```

---

## Naming Conventions

**These conventions are mandatory. Do NOT use FLTK-style naming.**

| Element | Convention | Example |
|---|---|---|
| Class name | PascalCase, noun | `GameController`, `BotStateMachine` |
| Method / function | camelCase, starts with verb | `updatePosition()`, `getHealth()` |
| Regular class field | `m_` prefix, camelCase | `m_health`, `m_position` |
| Pointer field | `mp_` prefix, camelCase | `mp_currentWeapon` |
| Static field | `s_` prefix, camelCase | `s_instanceCount` |
| Constant | UPPER_SNAKE_CASE | `MAX_HEALTH`, `TILE_SIZE` |
| Function argument | camelCase | `newHealth`, `weaponType` |
| Local variable | camelCase | `damageAmount`, `tileIndex` |
| Pointer variable | `p` prefix, camelCase | `pTarget`, `pCurrentTile` |
| Reference variable | `r` prefix, camelCase | `rPlayer`, `rGameState` |
| File name | snake_case, `.h` / `.cpp` | `game_object.h`, `bot_state_machine.cpp` |

---

## Architecture Details

### MVC Layer Responsibilities

#### Model (`src/model/`)

Contains all game data and logic. **Zero dependency on FLTK or View/Controller.**

**Class hierarchy:**

```
GameObject (abstract base)
├── Character (base for all characters: HP, armor, weapon, speed)
│   ├── Player (human-controlled)
│   └── Bot (AI-controlled, has FSM)
├── Bullet (projectile)
└── Bonus (abstract base for pickups)
    ├── HealthPack (+25 HP, max 100)
    ├── AmmoPack (refill current weapon ammo)
    └── ArmorPack (+50 armor, max 100; armor absorbs 50% damage)

Weapon (abstract base)
├── Pistol  (damage: 15, fire rate: 400ms, mag: 12, medium range)
├── Rifle   (damage: 25, fire rate: 150ms, mag: 30, high range)
└── Shotgun (damage: 8×6 pellets, fire rate: 800ms, mag: 8, low range)

GameMap       — tile-based map (2D array of Tile)
Tile          — single map cell (type, passability, coords)
Waypoint      — navigation point for bot pathfinding
GameState     — score, timer, game status (menu/playing/paused/game_over)
BotStateMachine — FSM for bot AI
CollisionManager — AABB collision detection
```

**OOP relationships (all four are required):**

| Relationship | Example | Notes |
|---|---|---|
| Inheritance | `Player : Character : GameObject` | Class hierarchy |
| Composition | `GameController` owns `GameState` | `GameState` cannot exist without `GameController` |
| Aggregation | `Character` has `Weapon` | `Weapon` can exist independently (pickup/drop) |
| Association | `Bot` uses `GameMap` for navigation | Independent objects interacting |

#### View (`src/view/`)

**Only layer that may use FLTK types and classes.**

| Class | Purpose |
|---|---|
| `MainWindow` | Main FLTK window (`Fl_Double_Window`). Created once, never destroyed until exit. |
| `MenuView` | Main menu screen (New Game / About / Exit buttons) |
| `GameView` | Game rendering widget (custom `draw()` override) |
| `HudView` | HUD overlay (HP bar, armor bar, ammo, score, timer) |
| `AboutView` | About screen (game info + controls) |
| `GameOverView` | End screen (final score + "Back to Menu" button) |

**FLTK rules:**
- Static callback functions are allowed **only** in View classes
- `void*` parameters are allowed **only** in static FLTK callback wrappers — never inside your own method calls
- A callback must be set by the class whose method will be invoked (no cross-class callback registration)
- Screen transitions (menu → game → game_over → menu) done by hiding/showing widgets, **not** by destroying/recreating the window

#### Controller (`src/controller/`)

| Class | Purpose |
|---|---|
| `GameController` | Main orchestrator: owns Model and View, coordinates everything |
| `InputController` | Captures keyboard/mouse input from View, translates to commands for Model |
| `GameLoopController` | Manages game loop, threads, timing |

---

### Multithreading Architecture

```
┌──────────────────────────────┐     ┌──────────────────────────────┐
│     MAIN THREAD (render)     │     │    LOGIC THREAD (update)     │
│                              │     │                              │
│  • FLTK event loop           │     │  • Update player position    │
│  • Fl::wait() / timer ~16ms  │     │  • Update bot AI (FSM)       │
│  • Read snapshot (mutex)     │     │  • Move bullets              │
│  • Draw map, objects, HUD    │     │  • Collision detection       │
│  • Capture input → queue     │     │  • Apply damage              │
│  • Fl::lock()/unlock()       │     │  • Spawn bots & bonuses      │
│                              │     │  • Update GameState           │
│  Target: ~60 FPS             │     │  • Write snapshot (mutex)    │
│                              │     │                              │
│                              │     │  Target: ~60 ticks/sec       │
└──────────┬───────────────────┘     └───────────────┬──────────────┘
           │                                         │
           └─────── shared: GameSnapshot (mutex) ─────┘
```

**Synchronization primitives:**

```cpp
std::mutex m_stateMutex;              // protects game snapshot
std::atomic<bool> m_isRunning;        // game loop flag
std::atomic<bool> m_isPaused;         // pause flag
std::condition_variable m_pauseCv;    // pause/resume logic thread
// FLTK thread safety:
Fl::lock();   // call in main() before Fl::run() to enable threading
Fl::unlock();
Fl::awake();  // wake up main thread from logic thread
```

**Thread lifecycle:**

1. `main()` calls `Fl::lock()` to enable FLTK multithreading
2. Main menu is shown (main thread only)
3. "New Game" → logic thread is created (`std::thread`)
4. Game loop: logic thread updates Model, main thread renders View
5. Game over → logic thread finishes, `join()`, return to menu
6. Exit → ensure logic thread is joined before `return 0`

**Critical: FLTK requires all GUI operations on the main thread.** The logic thread must never call FLTK drawing functions. Use `Fl::awake(callback, data)` to schedule GUI updates from the logic thread.

---

## Game Design

### Gameplay

- **Genre:** 2D top-down arcade shooter
- **Mode:** Deathmatch (free-for-all vs AI bots)
- **Camera:** top-down, centered on player
- **Objective:** score points by killing bots; game ends at score limit or time limit
- **Respawn:** player respawns after 3s, bots after 5s
- **Bots are always moving** (arcade requirement: constant motion)

### Controls

| Input | Action |
|---|---|
| W/A/S/D | Move up/left/down/right |
| Mouse move | Aim (rotate character) |
| Left click | Shoot |
| R | Reload |
| 1/2/3 | Switch weapon (if available) |
| Escape | Pause / return to menu |

### Weapons

| Weapon | Damage | Fire Rate | Magazine | Range | Spread |
|---|---|---|---|---|---|
| Pistol | 15 HP | 400ms | 12 | Medium | Low |
| Rifle | 25 HP | 150ms | 30 | High | Medium (increases when moving/firing) |
| Shotgun | 8 HP × 6 pellets | 800ms | 8 | Low | High (cone) |

Each weapon has a `m_spread` value — random bullet deviation from crosshair center. Spread increases while moving and during sustained fire.

### Bonuses

Spawn at random `BonusSpot` tiles periodically. Disappear after a timeout if not picked up.

| Bonus | Effect |
|---|---|
| HealthPack | +25 HP (max 100) |
| AmmoPack | Refill current weapon ammo |
| ArmorPack | +50 armor (max 100); armor absorbs 50% incoming damage |

### Character Stats

| Parameter | Player | Bot |
|---|---|---|
| HP | 100 | 100 |
| Armor | 0–100 | 0–100 |
| Speed | 200 px/s | 150–180 px/s |
| Starting weapon | Pistol | Random |
| Respawn time | 3s | 5s |

---

## Bot AI (Finite State Machine)

Bots use a 3-state FSM:

```
            [player visible]           [player in attack range]
  PATROL ──────────────────► CHASE ──────────────────────────► ATTACK
    ▲                          │                                  │
    │     [lost sight]         │        [player out of range]     │
    └──────────────────────────┘◄─────────────────────────────────┘
```

| State | Behavior |
|---|---|
| **Patrol** | Move between `Waypoint` nodes. Transition to Chase if player is detected (within `m_detectionRadius` and `m_fieldOfView`, with clear line of sight). |
| **Chase** | Move toward player's last known position. Transition to Attack if player is within weapon range. Return to Patrol if player is lost. |
| **Attack** | Shoot at player while maintaining optimal distance. Return to Chase if player moves out of sight. |

**AI parameters:**

```cpp
float m_detectionRadius = 300.0f;  // pixels
float m_fieldOfView = 120.0f;      // degrees
float m_attackRange;                // depends on bot's weapon
float m_accuracy;                   // 0.5–0.8 (reduces spread)
```

**Line of Sight:** raycast from bot to player; if ray intersects any `Wall` tile → no visibility.

**Navigation:** bots follow `Waypoint` nodes during patrol. During chase, use wall-sliding or simplified A* pathfinding.

---

## Tile Map

### Format

Maps are loaded from text files. Each cell is a tile code. Size: 32×32 pixels per tile.

```
1,1,1,1,1,1,1,1,1,1
1,0,0,0,0,0,0,0,0,1
1,0,1,1,0,0,1,1,0,1
1,0,0,2,0,0,3,0,0,1
1,0,1,0,4,0,0,1,0,1
1,0,0,0,0,0,0,0,0,1
1,0,1,0,0,4,0,1,0,1
1,0,0,3,0,0,0,0,0,1
1,0,0,0,0,0,0,0,0,1
1,1,1,1,1,1,1,1,1,1
```

| Code | Tile Type | Description |
|---|---|---|
| 0 | Floor | Passable |
| 1 | Wall | Blocks movement and bullets |
| 2 | SpawnPoint (player) | Player spawn location |
| 3 | SpawnPoint (bot) | Bot spawn location |
| 4 | BonusSpot | Potential bonus spawn point |

---

## Collision System

### Collision Types

| Pair | Response |
|---|---|
| Character ↔ Wall | Block movement (slide along wall) |
| Bullet ↔ Wall | Destroy bullet |
| Bullet ↔ Enemy Character | Deal damage, destroy bullet |
| Character ↔ Bonus | Apply bonus effect, destroy bonus |
| Character ↔ Character | Block (no clipping through each other) |

### Detection Method

- **Characters & Bonuses:** AABB intersection test
- **Bullets:** ray-segment intersection with AABBs of objects and wall tiles
- **Optimization:** only check collisions with objects in nearby tiles (grid-based spatial partitioning)

---

## Rendering

All drawing happens in `GameView::draw()` (overridden FLTK `draw()` method):

| Element | Rendering approach |
|---|---|
| Map tiles | `fl_rectf()` for walls (dark color), lighter color for floors. Only tiles visible on screen (frustum culling). |
| Characters | Colored circles (`fl_circle()`) with direction line (`fl_line()` toward aim). Player = green, Bots = red. |
| Bullets | Small colored dots or short lines |
| Bonuses | Colored squares with symbol (+, A, shield icon) |
| HUD | Drawn on top via `fl_draw()`, `fl_rectf()` — HP bar (red), armor bar (blue), ammo text, score, timer |

Camera follows player position. World-to-screen coordinate transform:
```cpp
screenX = worldX - cameraX + screenWidth / 2;
screenY = worldY - cameraY + screenHeight / 2;
```

Optional enhancement: load PNG textures via `Fl_PNG_Image` for sprites.

---

## UI Screens

### Main Menu
- **New Game** → start game (create logic thread)
- **About** → show AboutView
- **Exit** → quit application

### Game Screen
- GameView (game field with camera)
- HudView overlay (HP, armor, ammo, score, timer, kill messages)

### About Screen
- Game title, developer name, group number
- Controls table
- "Back" button → return to menu

### Game Over Screen
- Final score and elapsed time
- "Back to Menu" button → return to main menu

**Screen transitions:** hide/show FLTK widget groups. **Never** destroy and recreate `MainWindow`.

---

## Memory Management Rules

1. **Default to `std::unique_ptr`** for exclusive ownership:
   ```cpp
   std::vector<std::unique_ptr<Bullet>> m_bullets;
   std::unique_ptr<Weapon> m_pWeapon;
   ```

2. **Use `std::shared_ptr`** only when multiple owners genuinely need shared ownership:
   ```cpp
   std::shared_ptr<GameMap> m_pMap; // shared between Model and Bots
   ```

3. **Non-owning references:** raw `T*` or `std::weak_ptr` (never `std::shared_ptr` for non-owning refs)

4. **Every `new` must be justified** — "because the example did it" is not valid

5. **No global variables** (only `main()` function is the exception)

6. **No memory leaks** — verify with Valgrind or AddressSanitizer

---

## Build System (CMake)

Minimum `CMakeLists.txt` requirements:

```cmake
cmake_minimum_required(VERSION 3.16)
project(CS2DS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLTK REQUIRED)

# Collect sources by layer
file(GLOB_RECURSE MODEL_SRC   src/model/*.cpp)
file(GLOB_RECURSE VIEW_SRC    src/view/*.cpp)
file(GLOB_RECURSE CTRL_SRC    src/controller/*.cpp)

add_executable(cs2ds
    src/main.cpp
    ${MODEL_SRC}
    ${VIEW_SRC}
    ${CTRL_SRC}
)

target_include_directories(cs2ds PRIVATE src ${FLTK_INCLUDE_DIR})
target_link_libraries(cs2ds PRIVATE ${FLTK_LIBRARIES} pthread)

# Copy assets to build directory automatically
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
```

**Critical:** all assets must be automatically copied to the build directory. No manual "copy this file to that folder" instructions.

---

## Quality Requirements

- [ ] No memory leaks (Valgrind / ASan clean)
- [ ] No crashes or unhandled exceptions
- [ ] Threads properly joined on exit
- [ ] Stable ~60 FPS with up to 10 bots
- [ ] Characters never clip through walls
- [ ] Menu ↔ Game transitions work without window recreation
- [ ] All four OOP relationships present
- [ ] All class fields are `private`
- [ ] No FLTK types outside View layer
- [ ] No `void*` outside FLTK callback wrappers
- [ ] No global variables
- [ ] Smart pointers used everywhere (no raw `new`/`delete`)

---

## Development Phases

| Phase | Task | Deliverable |
|---|---|---|
| 1 | CMake setup, FLTK integration, directory structure, empty window with main menu | Window with working menu (New Game, About, Exit) |
| 2 | Model: `GameObject`, `Character`, `Player`, `GameMap`, `Tile`. Load map from file. | Map loaded in memory, Player class exists |
| 3 | View: render map and player. Camera centered on player. WASD + mouse control. | Player moves around the map with camera |
| 4 | Weapons: `Weapon`, `Pistol`, `Rifle`, `Shotgun`, `Bullet`. Bullet ↔ wall collision. | Working shooting, bullets destroyed on walls |
| 5 | Bot AI: `Bot`, `BotStateMachine`. Patrol/Chase/Attack. Bullet ↔ character collision. | Bots move and shoot |
| 6 | Bonuses: `HealthPack`, `AmmoPack`, `ArmorPack`. Spawning and pickup. | Bonuses appear and can be collected |
| 7 | Multithreading: move logic to separate thread. Mutex/snapshot sync. | Two threads running correctly |
| 8 | HUD, game over screen, about screen. Menu ↔ game transition without window recreation. | Full UI |
| 9 | Replace any remaining raw pointers with smart pointers. Code review for naming conventions. Testing. | Final polished version |
| 10 | Publish to GitVerse. Create pull request assigned to instructor. | Ready for submission |