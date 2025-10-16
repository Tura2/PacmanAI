#include <cstdlib>
#include <queue>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include "glut.h"

#include "include/Definitions.h"
#include "include/Grid.h"
#include "include/Pacman.h"
#include "include/Ghost.h"

static Grid   g_grid;
static Pacman g_pac(PR, PC);
static std::vector<Ghost> g_ghosts;

static int  g_collected = 0;
static const int FRAME_MS = 100;

enum class GameState { RUNNING, GAME_OVER };
static GameState g_state = GameState::RUNNING;

static void Setup2D() {
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0.0, MSZ, 0.0, MSZ);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void DrawText(float x, float y, const std::string& s, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (unsigned char ch : s) glutBitmapCharacter(font, ch);
}

// Snap to nearest walkable tile via BFS
static std::pair<int, int> NearestFree(const Grid& grid, int sr, int sc) {
    if (grid.inBounds(sr, sc) && grid.isFree(sr, sc)) return { sr,sc };
    static const int DR[4] = { -1,1,0,0 };
    static const int DC[4] = { 0,0,-1,1 };
    bool vis[MSZ][MSZ] = {};
    std::queue<std::pair<int, int>> q;
    auto push = [&](int r, int c) { if (grid.inBounds(r, c) && !vis[r][c]) { vis[r][c] = true; q.push({ r,c }); } };
    if (grid.inBounds(sr, sc)) push(sr, sc);
    while (!q.empty()) {
        auto [r, c] = q.front(); q.pop();
        if (grid.isFree(r, c)) return { r,c };
        for (int k = 0;k < 4;++k) push(r + DR[k], c + DC[k]);
    }
    return { sr,sc };
}

static int clampi(int v, int lo, int hi) { return std::max(lo, std::min(v, hi)); }

static void ResetGame() {
    g_grid.InitMaze();
    g_collected = 0;

    // Pac-Man spawn (snap to walkable)
    auto ppos = NearestFree(g_grid, PR, PC);
    g_pac.setPos(ppos.first, ppos.second);

    // ---- Three ghosts at requested positions ----
    // 1) (TR, TC)
    int r1 = TR, c1 = TC;

    // 2) (TR/2, TC/2)
    int r2 = TR / 2, c2 = TC / 2;

    // 3) (TR*2, TC)  (clamp inside the board interior, then snap to free)
    int r3 = clampi(TR * 2, 1, MSZ - 2);
    int c3 = clampi(TC, 1, MSZ - 2);

    auto g1 = NearestFree(g_grid, r1, c1);
    auto g2 = NearestFree(g_grid, r2, c2);
    auto g3 = NearestFree(g_grid, r3, c3);

    g_ghosts.clear();
    g_ghosts.emplace_back(g1.first, g1.second);
    g_ghosts.emplace_back(g2.first, g2.second);
    g_ghosts.emplace_back(g3.first, g3.second);

    g_state = GameState::RUNNING;
}

static void DrawHUD() {
    glColor3f(1, 1, 0);
    std::string msg = "Coins: " + std::to_string(g_collected) + " / " + std::to_string(g_grid.coinCount() + g_collected);
    DrawText(1.0f, MSZ - 1.5f, msg);

    if (g_state == GameState::GAME_OVER) {
        glColor3f(1.0f, 0.3f, 0.3f);
        DrawText(MSZ / 2.0f - 6.0f, MSZ / 2.0f, "GAME OVER");
        glColor3f(1.0f, 1.0f, 1.0f);
        DrawText(MSZ / 2.0f - 12.0f, MSZ / 2.0f - 2.0f, "Press SPACE to restart");
    }
}

static void Display() {
    glClearColor(0.0f, 0.0f, 0.06f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    g_grid.Draw();
    g_pac.draw();
    for (auto& gh : g_ghosts) gh.draw();

    DrawHUD();
    glutSwapBuffers();
}

static void OnTimer(int) {
    if (g_state == GameState::RUNNING) {
        // Build list of ghost positions for Pac-Man AI
        std::vector<std::pair<int, int>> ghostsPos;
        ghostsPos.reserve(g_ghosts.size());
        for (auto& gh : g_ghosts) ghostsPos.emplace_back(gh.getRow(), gh.getCol());

        // Update AIs
        g_pac.updateAI(g_grid, ghostsPos);
        for (auto& gh : g_ghosts) gh.updateWithGridAndPac(g_grid, g_pac);

        // Coin pickup
        if (g_grid.pickCoin(g_pac.getRow(), g_pac.getCol()))
            ++g_collected;

        // Collision: any ghost on Pac-Man → game over
        for (auto& gh : g_ghosts) {
            if (g_pac.getRow() == gh.getRow() && g_pac.getCol() == gh.getCol()) {
                g_state = GameState::GAME_OVER;
                break;
            }
        }
    }
    glutPostRedisplay();
    glutTimerFunc(FRAME_MS, OnTimer, 0);
}

static void Reshape(int w, int h) { glViewport(0, 0, w, h); Setup2D(); }

static void Keyboard(unsigned char key, int, int) {
    if (key == 27 || key == 'q' || key == 'Q') std::exit(0);
    if (key == ' ' && g_state == GameState::GAME_OVER) ResetGame();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Pacman AI — 3 Ghosts (custom spawns)");
    ResetGame();
    Setup2D();
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(FRAME_MS, OnTimer, 0);
    glutMainLoop();
    return 0;
}
