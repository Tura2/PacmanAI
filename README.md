# 🟡 PacmanAI
**Pac-Man AI — BFS vs A* Chase in a Procedural Maze (C++ / OpenGL)**

---

## 🧠 Overview
AI-driven Pac-Man clone using **FreeGLUT** and classic **OpenGL**.  
Designed to demonstrate **grid-based pathfinding** and simple **reactive AI**.

---

## 🎯 Features

### 🟡 Pac-Man AI
- Seeks nearest coin using **BFS (limited depth)**
- Switches to **FLEE** mode when a ghost approaches
- Resumes coin collection when safe

### 👻 Ghost AI
- Uses **A\*** pathfinding to chase Pac-Man
- Moves and replans at controlled intervals for smoother pursuit

### 🧱 Grid System
- Procedural **parity-based maze generation**
- Coins placed **only on reachable tiles**
- Efficient rendering using **immediate-mode OpenGL**

### 🖥️ HUD
- Displays collected coins and game-over message

---

## 🎮 Controls

| Key | Action |
|-----|---------|
| **SPACE** | Restart after game over |
| **Q / ESC** | Quit the game |

---

## 📁 File Structure

<pre><code>include/
  Definitions.h   # Constants and tuning parameters
  Entity.h        # Base class for drawable/updateable entities
  Grid.h          # Maze generation, coin logic, rendering
  Ghost.h         # Ghost class (A* logic)
  Pacman.h        # Pac-Man AI (BFS + flee)
  Pathfinding.h   # Cell type + pathfinding utilities

src/
  Grid.cpp        # Maze + coin generation and draw
  Ghost.cpp       # A* ghost movement
  Pacman.cpp      # BFS/flee Pac-Man logic
  Pathfinding.cpp # BFS, A*, helpers
  main.cpp        # Game loop, rendering, input, HUD
</code></pre>

---

## ⚙️ Tuning Parameters
Defined in `include/Definitions.h`:

| Parameter | Description |
|------------|-------------|
| `PACMAN_DEPTH_LIMIT` | BFS depth for Pac-Man’s search |
| `FLEE_TRIGGER_DIST` / `FLEE_EXIT_DIST` | Distance thresholds for flee mode |
| `GHOST_STEP_PERIOD`, `GHOST_REPLAN_PERIOD` | Ghost pacing and replan frequency |
| `WALL_PROB` | Wall density bias in maze generation |
| `MSZ` | Grid size (affects game scale and performance) |

---

## 🧩 Notes
- Uses **fixed-function OpenGL** for simplicity (no shaders).  
- Works on any **GLUT-compatible platform**.  
- Behavior and visuals are tuned for **clarity and educational demonstration**, not realism.

---

### 👾 Enjoy watching the chase!
