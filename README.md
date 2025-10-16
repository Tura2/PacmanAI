# PacmanAI
Pac-Man AI — BFS vs A* chase in a procedural maze (C++ / OpenGL)

## Overview
AI-driven Pac-Man clone using FreeGLUT and classic OpenGL.  
Designed to demonstrate grid-based pathfinding and simple reactive AI.

### Features
- **Pac-Man AI**
  - Seeks nearest coin via BFS (limited depth).
  - Switches to FLEE mode when a ghost is close.
  - Resumes collecting when safe.
- **Ghost AI**
  - Uses A* pathfinding to chase Pac-Man.
  - Moves and replans at controlled intervals.
- **Grid System**
  - Procedural maze generation (parity-based).
  - Coins placed only on reachable tiles.
  - Efficient rendering via immediate-mode OpenGL.
- **HUD**
  - Displays collected coins and restart message.

## Controls
- **SPACE** → Restart after game over  
- **Q / ESC** → Quit the game

## File Structure
include/
      Definitions.h # Constants and tuning parameters
      Entity.h # Base class for drawable/updateable entities
      Grid.h # Maze generation, coin logic, rendering
      Ghost.h # Ghost class (A* logic)
      Pacman.h # Pac-Man AI (BFS + flee)
      Pathfinding.h # Cell type + pathfinding utilities
src/
      Grid.cpp # Maze + coin generation and draw
      Ghost.cpp # A* ghost movement
      Pacman.cpp # BFS/flee Pac-Man logic
      Pathfinding.cpp # BFS, A*, helpers
      main.cpp # Game loop, rendering, input, HUD

## Tuning Parameters
Defined in `include/Definitions.h`:
- `PACMAN_DEPTH_LIMIT` — BFS depth for Pac-Man  
- `FLEE_TRIGGER_DIST` / `FLEE_EXIT_DIST` — ghost proximity thresholds  
- `GHOST_STEP_PERIOD`, `GHOST_REPLAN_PERIOD` — ghost pacing  
- `WALL_PROB` — wall density bias  
- `MSZ` — grid size (affects performance and visibility)

## Notes
- Fixed-function OpenGL (no shaders) for simplicity.
- Works on any GLUT-compatible platform.
- Behavior and visuals tuned for clarity, not realism.

Enjoy watching the chase!
