#pragma once
#include "Entity.h"
#include <vector>        // for cached path
#include "Pathfinding.h" // Cell

class Grid; class Pacman;

class Ghost : public Entity {
public:
    Ghost(int r = 0, int c = 0) : row(r), col(c) {}

    void update(float dt) override;
    void draw() const override;

    void updateWithGridAndPac(const Grid& g, const Pacman& pac);

    int  getRow() const { return row; }
    int  getCol() const { return col; }
    void setPos(int r, int c) { row = r; col = c; }

private:
    int row, col;

    // pacing / caching
    int stepTick = 0;
    int replanTick = 0;
    std::vector<Cell> cachedPath;
};
