#include <cmath>
#include "glut.h"
#include "../include/Pacman.h"
#include "../include/Definitions.h"
#include "../include/Pathfinding.h"
#include "../include/Grid.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void Pacman::update(float /*dt*/) {}

void Pacman::updateWithGrid(const Grid& g)
{
    if (currPath.size() <= 1 || thinkCooldown == 0) {
        auto p = bfsLimited(g, row, col, TR, TC, PACMAN_DEPTH_LIMIT);
        if (!p.empty()) currPath = std::move(p);
        thinkCooldown = 3;
    }
    else --thinkCooldown;

    if (currPath.size() > 1) {
        row = currPath[1].r; col = currPath[1].c;
        currPath.erase(currPath.begin());
    }
}

static int opennessScore(const Grid& g, int r, int c)
{
    static const int DR[4] = { -1,1,0,0 };
    static const int DC[4] = { 0,0,-1,1 };
    int score = 0;
    for (int k = 0;k < 4;++k) {
        int nr = r + DR[k], nc = c + DC[k];
        if (g.inBounds(nr, nc) && g.isFree(nr, nc)) ++score;
    }
    return score;
}

void Pacman::updateAI(const Grid& g, const std::vector<std::pair<int, int>>& ghosts)
{
    int dGhost = nearestGhostDist(row, col, ghosts);
    if (mode == COINS) {
        if (dGhost <= FLEE_TRIGGER_DIST) { mode = FLEE; fleeCooldown = FLEE_COOLDOWN_STEPS; currPath.clear(); }
    }
    else {
        if (fleeCooldown > 0) --fleeCooldown;
        if (dGhost >= FLEE_EXIT_DIST && fleeCooldown == 0) { mode = COINS; currPath.clear(); }
    }

    if (currPath.size() <= 1 || thinkCooldown == 0)
    {
        if (mode == FLEE) {
            static const int DR[4] = { -1,1,0,0 };
            static const int DC[4] = { 0,0,-1,1 };
            int bestR = row, bestC = col;
            int bestVal = -1e9;

            for (int k = 0;k < 4;++k) {
                int nr = row + DR[k], nc = col + DC[k];
                if (!g.inBounds(nr, nc) || !g.isFree(nr, nc)) continue;
                int dist = nearestGhostDist(nr, nc, ghosts);
                int open = opennessScore(g, nr, nc);
                int val = dist * 10 + open;
                if (val > bestVal) { bestVal = val; bestR = nr; bestC = nc; }
            }
            currPath = { {row,col} };
            if (bestR != row || bestC != col) currPath.push_back({ bestR,bestC });
        }
        else {
            auto p = bfsToNearestCoinLimited(g, row, col, PACMAN_DEPTH_LIMIT);
            if (!p.empty()) currPath = std::move(p);
            else {
                static const int DR[4] = { -1,1,0,0 };
                static const int DC[4] = { 0,0,-1,1 };
                for (int k = 0;k < 4;++k) {
                    int nr = row + DR[k], nc = col + DC[k];
                    if (g.inBounds(nr, nc) && g.isFree(nr, nc)) { currPath = { {row,col},{nr,nc} }; break; }
                }
            }
        }
        thinkCooldown = 2;
    }
    else --thinkCooldown;

    if (currPath.size() > 1) {
        row = currPath[1].r; col = currPath[1].c;
        currPath.erase(currPath.begin());
    }
}

void Pacman::draw() const
{
    const float cx = (float)col + 0.5f, cy = (float)row + 0.5f, radius = 0.42f, mouthDeg = 35.0f;
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (float a = mouthDeg; a <= 360.0f - mouthDeg; a += 5.0f) {
        float t = a * (float)M_PI / 180.0f;
        glVertex2f(cx + radius * cosf(t), cy + radius * sinf(t));
    }
    glEnd();
}
