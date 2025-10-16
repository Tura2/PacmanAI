#pragma once
#include "Entity.h"
#include <vector>
#include "Pathfinding.h"

class Grid;

class Pacman : public Entity {
public:
    Pacman(int r = 0, int c = 0) : row(r), col(c) {}

    void update(float dt) override;
    void draw() const override;

    void updateAI(const Grid& g, const std::vector<std::pair<int, int>>& ghosts);
    void updateWithGrid(const Grid& g);

    int  getRow() const { return row; }
    int  getCol() const { return col; }
    void setPos(int r, int c) { row = r; col = c; } 

private:
    enum Mode { FLEE, COINS } mode = COINS;

    int row, col;
    int thinkCooldown = 0;
    int fleeCooldown = 0;
    std::vector<Cell> currPath;
};
