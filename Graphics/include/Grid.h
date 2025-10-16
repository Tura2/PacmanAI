#pragma once
#include "Definitions.h"

class Grid {
public:
    Grid() = default;

    // Build a randomized maze and coin field
    void InitMaze();
    void Draw() const;

    // Logic helpers
    int  at(int r, int c) const;        // returns cell type
    bool inBounds(int r, int c) const;  // 0..MSZ-1
    bool isFree(int r, int c) const;    // SPACE or TARGET (walkable)

    // Coins
    bool hasCoin(int r, int c) const;
    bool pickCoin(int r, int c);        // <-- return true if a coin was removed
    int  coinCount() const { return totalCoins; }

private:
    int  maze[MSZ][MSZ] = {};
    bool coins[MSZ][MSZ] = {};
    int  totalCoins = 0;

    void scatterCoins();
};
