#pragma once

// ===== Core board =====
constexpr int MSZ = 50;

enum CellType { SPACE, WALL, START, TARGET, PATH };

// ===== Window (GLUT) =====
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;

// ===== Spawns (well-separated) =====
constexpr int PR = MSZ / 2;            // 20
constexpr int PC = MSZ / 2 + 16;       // 36 (right side, inside border)
constexpr int GR = MSZ / 2;            // 20
constexpr int GC = MSZ / 2 - 12;       // 8  (left of center)

// ===== Target (visual only) =====
constexpr int TR = MSZ / 2 + 10;       // 30
constexpr int TC = MSZ / 2;            // 20

// ===== Pacman behavior (good advantage on 40x40) =====
constexpr int PACMAN_DEPTH_LIMIT = 50; // strong lookahead
constexpr int FLEE_TRIGGER_DIST = 4;   
constexpr int FLEE_EXIT_DIST = 4;      
constexpr int FLEE_COOLDOWN_STEPS = 5; // avoid mode flip-flop

// ===== Ghost pacing (fair) =====
constexpr int GHOST_STEP_PERIOD = 2;    // ghost moves every other tick
constexpr int GHOST_REPLAN_PERIOD = 6;  // replan A* every other tick

// ===== Randomization knob (Grid may ignore if using parity maze) =====
constexpr float WALL_PROB = 0.22f;
