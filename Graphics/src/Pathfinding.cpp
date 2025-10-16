#include "../include/Pathfinding.h"
#include "../include/Grid.h"

#include <queue>
#include <vector>
#include <algorithm>
#include <limits>
#include <tuple>
#include <cmath>

bool inBounds(int r, int c) { return r >= 0 && r < MSZ && c >= 0 && c < MSZ; }
bool isFree(const Grid& g, int r, int c) { return g.isFree(r, c); }

int manhattan(int r1, int c1, int r2, int c2) { return std::abs(r1 - r2) + std::abs(c1 - c2); }

int nearestGhostDist(int r, int c, const std::vector<std::pair<int, int>>& ghosts)
{
    int best = std::numeric_limits<int>::max();
    for (auto& g : ghosts) best = std::min(best, manhattan(r, c, g.first, g.second));
    return best;
}

std::vector<Cell> bfsLimited(const Grid& g, int sr, int sc, int tr, int tc, int depthLimit)
{
    struct Node { int r, c, d; };
    bool vis[MSZ][MSZ] = {};
    int  pr[MSZ][MSZ], pc[MSZ][MSZ];
    for (int r = 0;r < MSZ;++r) for (int c = 0;c < MSZ;++c) pr[r][c] = pc[r][c] = -1;

    if (!g.inBounds(sr, sc) || !g.inBounds(tr, tc)) return {};
    std::queue<Node> q; q.push({ sr,sc,0 }); vis[sr][sc] = true;

    const int dr[4] = { -1,1,0,0 }, dc[4] = { 0,0,-1,1 };
    bool found = false; int er = -1, ec = -1;

    while (!q.empty()) {
        Node cur = q.front(); q.pop();
        if (cur.r == tr && cur.c == tc) { found = true; er = cur.r; ec = cur.c; break; }
        if (cur.d == depthLimit) continue;
        for (int k = 0;k < 4;++k) {
            int nr = cur.r + dr[k], nc = cur.c + dc[k];
            if (g.inBounds(nr, nc) && g.isFree(nr, nc) && !vis[nr][nc]) {
                vis[nr][nc] = true; pr[nr][nc] = cur.r; pc[nr][nc] = cur.c;
                q.push({ nr,nc,cur.d + 1 });
            }
        }
    }

    std::vector<Cell> path;
    if (!found) return path;
    for (int r = er, c = ec; r != -1 && c != -1; ) { path.push_back({ r,c }); int rr = pr[r][c], cc = pc[r][c]; r = rr; c = cc; }
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<Cell> astarPath(const Grid& g, int sr, int sc, int tr, int tc)
{
    auto H = [tr, tc](int r, int c) { return std::abs(r - tr) + std::abs(c - tc); };
    const int INF = std::numeric_limits<int>::max() / 4;
    int G[MSZ][MSZ], pr[MSZ][MSZ], pc[MSZ][MSZ];
    for (int r = 0;r < MSZ;++r) for (int c = 0;c < MSZ;++c) { G[r][c] = INF; pr[r][c] = pc[r][c] = -1; }

    if (!g.inBounds(sr, sc) || !g.inBounds(tr, tc)) return {};
    using Item = std::tuple<int, int, int>; // f,r,c
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open;

    G[sr][sc] = 0; open.emplace(H(sr, sc), sr, sc);
    const int dr[4] = { -1,1,0,0 }, dc[4] = { 0,0,-1,1 };

    while (!open.empty()) {
        auto [f, r, c] = open.top(); open.pop();
        if (r == tr && c == tc) {
            std::vector<Cell> path;
            for (int R = r, C = c; R != -1 && C != -1; ) { path.push_back({ R,C }); int RR = pr[R][C], CC = pc[R][C]; R = RR; C = CC; }
            std::reverse(path.begin(), path.end()); return path;
        }
        for (int k = 0;k < 4;++k) {
            int nr = r + dr[k], nc = c + dc[k];
            if (!g.inBounds(nr, nc) || !g.isFree(nr, nc)) continue;
            int t = G[r][c] + 1;
            if (t < G[nr][nc]) { G[nr][nc] = t; pr[nr][nc] = r; pc[nr][nc] = c; open.emplace(t + H(nr, nc), nr, nc); }
        }
    }
    return {};
}

std::vector<Cell> bfsToNearestCoinLimited(const Grid& g, int sr, int sc, int depthLimit)
{
    struct Node { int r, c, d; };
    static const int DR[4] = { -1,1,0,0 };
    static const int DC[4] = { 0,0,-1,1 };

    // Collect current coin positions once
    std::vector<Cell> coinPos;
    coinPos.reserve(MSZ * MSZ / 2);
    for (int r = 0; r < MSZ; ++r)
        for (int c = 0; c < MSZ; ++c)
            if (g.hasCoin(r, c)) coinPos.push_back({ r,c });

    auto distToNearestCoin = [&](int r, int c) {
        int best = INT_MAX;
        for (const auto& co : coinPos)
            best = std::min(best, manhattan(r, c, co.r, co.c));
        return best;
        };

    auto openness = [&](int r, int c) {
        int cnt = 0;
        for (int k = 0;k < 4;++k) {
            int nr = r + DR[k], nc = c + DC[k];
            if (g.inBounds(nr, nc) && g.isFree(nr, nc)) ++cnt;
        }
        return cnt; // more exits => safer
        };

    bool vis[MSZ][MSZ] = {};
    int  PRV[MSZ][MSZ], PCV[MSZ][MSZ];
    for (int r = 0;r < MSZ;++r) for (int c = 0;c < MSZ;++c) PRV[r][c] = PCV[r][c] = -1;

    std::queue<Node> q;
    if (!(g.inBounds(sr, sc) && g.isFree(sr, sc))) return {};
    q.push({ sr,sc,0 }); vis[sr][sc] = true;

    // Track best "frontier progress" if we don't find a coin within depth
    bool haveBestFrontier = false;
    int  bestFR = sr, bestFC = sc;
    int  bestFValDist = INT_MAX;
    int  bestFValOpen = -1;
    int  bestFDepth = INT_MAX;

    // If we are already on a coin, trivial path
    if (g.hasCoin(sr, sc)) return { {sr,sc} };

    // BFS up to depthLimit
    while (!q.empty()) {
        Node cur = q.front(); q.pop();

        // If we reached a coin within depth, reconstruct path immediately
        if (g.hasCoin(cur.r, cur.c)) {
            std::vector<Cell> path;
            for (int r = cur.r, c = cur.c; r != -1 && c != -1; ) {
                path.push_back({ r,c });
                int rr = PRV[r][c], cc = PCV[r][c];
                r = rr; c = cc;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        if (cur.d == depthLimit) {
            // frontier node: score it for "best progress toward a coin"
            int dCoin = distToNearestCoin(cur.r, cur.c);
            int open = openness(cur.r, cur.c);
            // Minimize dCoin; tie-break by max openness; then by shallower depth
            bool better = false;
            if (dCoin < bestFValDist) better = true;
            else if (dCoin == bestFValDist && open > bestFValOpen) better = true;
            else if (dCoin == bestFValDist && open == bestFValOpen && cur.d < bestFDepth) better = true;

            if (better) {
                haveBestFrontier = true;
                bestFValDist = dCoin;
                bestFValOpen = open;
                bestFDepth = cur.d;
                bestFR = cur.r; bestFC = cur.c;
            }
            continue;
        }

        for (int k = 0;k < 4;++k) {
            int nr = cur.r + DR[k], nc = cur.c + DC[k];
            if (!g.inBounds(nr, nc) || !g.isFree(nr, nc) || vis[nr][nc]) continue;
            vis[nr][nc] = true; PRV[nr][nc] = cur.r; PCV[nr][nc] = cur.c;
            q.push({ nr,nc,cur.d + 1 });
        }
    }

    if (haveBestFrontier) {
        std::vector<Cell> path;
        for (int r = bestFR, c = bestFC; r != -1 && c != -1; ) {
            path.push_back({ r,c });
            int rr = PRV[r][c], cc = PCV[r][c];
            r = rr; c = cc;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    return {};
}
