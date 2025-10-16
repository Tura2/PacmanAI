#include "glut.h"
#include "../include/Grid.h"
#include "../include/Definitions.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <queue>

// ---------- helpers ----------
static bool rndBelow(float p) { return (std::rand() / (float)RAND_MAX) < p; }
static int clampi(int v, int lo, int hi) { return std::max(lo, std::min(v, hi)); }

// Coins on reachable SPACE with probability.
void Grid::scatterCoins()
{
    const float COIN_PROB = 0.25f;

    auto inB = [this](int r, int c) { return inBounds(r, c); };
    auto walk = [this](int r, int c) {
        if (!inBounds(r, c)) return false;
        int v = maze[r][c];
        return v == SPACE || v == START || v == TARGET;
        };

    // Find nearest free to (PR,PC) for robust start
    auto nearestFree = [&](int sr, int sc) {
        if (walk(sr, sc)) return std::make_pair(sr, sc);
        static const int DR[4] = { -1,1,0,0 };
        static const int DC[4] = { 0,0,-1,1 };
        bool vis[MSZ][MSZ] = {};
        std::queue<std::pair<int, int>> q;
        if (inB(sr, sc)) { vis[sr][sc] = true; q.push({ sr, sc }); }
        while (!q.empty()) {
            auto [r, c] = q.front(); q.pop();
            if (walk(r, c)) return std::make_pair(r, c);
            for (int k = 0; k < 4; ++k) {
                int nr = r + DR[k], nc = c + DC[k];
                if (inB(nr, nc) && !vis[nr][nc]) { vis[nr][nc] = true; q.push({ nr, nc }); }
            }
        }
        return std::make_pair(sr, sc);
        };

    int startR = PR, startC = PC;
    std::tie(startR, startC) = nearestFree(startR, startC);

    // BFS reachability from start
    static const int DR[4] = { -1, 1, 0, 0 };
    static const int DC[4] = { 0, 0,-1, 1 };
    bool reach[MSZ][MSZ] = {};
    std::queue<std::pair<int, int>> q;

    if (walk(startR, startC)) {
        reach[startR][startC] = true;
        q.push({ startR, startC });
    }

    while (!q.empty()) {
        auto [r, c] = q.front(); q.pop();
        for (int k = 0; k < 4; ++k) {
            int nr = r + DR[k], nc = c + DC[k];
            if (inB(nr, nc) && !reach[nr][nc] && walk(nr, nc)) {
                reach[nr][nc] = true;
                q.push({ nr, nc });
            }
        }
    }

    // Place coins ONLY on reachable SPACE cells
    totalCoins = 0;
    for (int r = 0; r < MSZ; ++r) {
        for (int c = 0; c < MSZ; ++c) {
            coins[r][c] = false;
            if (maze[r][c] == SPACE && reach[r][c] && rndBelow(COIN_PROB)) {
                coins[r][c] = true;
                ++totalCoins;
            }
        }
    }

    // Remove coins under typical spawns and fix the counter if needed
    auto clearCoin = [&](int r, int c) {
        if (inB(r, c) && coins[r][c]) { coins[r][c] = false; --totalCoins; }
        };
    clearCoin(GR, GC);
    clearCoin(startR, startC);
}

// ---------- InitMaze (parity-based + coins) ----------
void Grid::InitMaze()
{
    std::srand((unsigned)std::time(nullptr));

    // 0) clear
    for (int r = 0; r < MSZ; ++r)
        for (int c = 0; c < MSZ; ++c)
            maze[r][c] = SPACE;

    // 1) frame
    for (int i = 0; i < MSZ; ++i) {
        maze[0][i] = WALL;
        maze[MSZ - 1][i] = WALL;
        maze[i][0] = WALL;
        maze[i][MSZ - 1] = WALL;
    }

    // 2) parity-biased interior
    for (int i = 1; i < MSZ - 1; ++i)
        for (int j = 1; j < MSZ - 1; ++j) {
            if ((i % 2) == 0) {
                maze[i][j] = (std::rand() % 100 < 55) ? WALL : SPACE;
            }
            else {
                maze[i][j] = (std::rand() % 100 < 90) ? SPACE : WALL;
            }
        }

    // 3) re-assert border (defensive)
    for (int i = 0; i < MSZ; ++i) {
        maze[0][i] = WALL; maze[MSZ - 1][i] = WALL;
        maze[i][0] = WALL; maze[i][MSZ - 1] = WALL;
    }

    // 4) coins on reachable cells
    scatterCoins();

    // 5) optional markers
    if (PR >= 0 && PR < MSZ && PC >= 0 && PC < MSZ) maze[PR][PC] = START;
    if (TR >= 0 && TR < MSZ && TC >= 0 && TC < MSZ) maze[TR][TC] = TARGET;
}

// ---------- draw & queries ----------
void Grid::Draw() const
{
    for (int r = 0; r < MSZ; ++r)
    {
        for (int c = 0; c < MSZ; ++c)
        {
            const int cell = maze[r][c];
            switch (cell)
            {
            case SPACE:  glColor3f(0.0f, 0.0f, 0.0f); break;
            case WALL:   glColor3f(0.0f, 0.0f, 0.5f); break;
            case START:  glColor3f(0.0f, 0.0f, 0.25f); break;
            case TARGET: glColor3f(0.7f, 0.0f, 0.0f); break;
            case PATH:   glColor3f(1.0f, 0.0f, 1.0f); break;
            default:     glColor3f(0.0f, 0.0f, 0.0f); break;
            }

            glBegin(GL_QUADS);
            glVertex2f((float)c, (float)r);
            glVertex2f((float)c + 1, (float)r);
            glVertex2f((float)c + 1, (float)r + 1);
            glVertex2f((float)c, (float)r + 1);
            glEnd();

            if (coins[r][c]) {
                glColor3f(1.0f, 1.0f, 0.7f);
                const float cx = c + 0.5f, cy = r + 0.5f, rad = 0.06f;
                glBegin(GL_TRIANGLE_FAN);
                glVertex2f(cx, cy);
                for (int i = 0; i <= 20; ++i) {
                    const float t = 2.0f * 3.1415926f * i / 20.0f;
                    glVertex2f(cx + rad * cosf(t), cy + rad * sinf(t));
                }
                glEnd();
            }
        }
    }
}

int  Grid::at(int r, int c) const { return maze[r][c]; }
bool Grid::inBounds(int r, int c) const { return r >= 0 && r < MSZ && c >= 0 && c < MSZ; }
bool Grid::isFree(int r, int c) const { int v = maze[r][c]; return v == SPACE || v == TARGET || v == START; }
bool Grid::hasCoin(int r, int c) const { return inBounds(r, c) && coins[r][c]; }
bool Grid::pickCoin(int r, int c) { if (hasCoin(r, c)) { coins[r][c] = false; --totalCoins; return true; } return false; }
