#pragma once
#include <vector>
#include <utility>

class Grid;

struct Cell { int r; int c; };

// Existing
bool inBounds(int r, int c);
bool isFree(const Grid& g, int r, int c);
std::vector<Cell> bfsLimited(const Grid& g, int sr, int sc, int tr, int tc, int depthLimit);
std::vector<Cell> astarPath(const Grid& g, int sr, int sc, int tr, int tc);

// New helpers
int manhattan(int r1, int c1, int r2, int c2);
int nearestGhostDist(int r, int c, const std::vector<std::pair<int, int>>& ghosts);
std::vector<Cell> bfsToNearestCoinLimited(const Grid& g, int sr, int sc, int depthLimit);
