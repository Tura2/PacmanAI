#include <cmath>
#include "glut.h"
#include "../include/Ghost.h"
#include "../include/Grid.h"
#include "../include/Pacman.h"
#include "../include/Definitions.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void Ghost::update(float /*dt*/) { /* unused */ }

void Ghost::updateWithGridAndPac(const Grid& g, const Pacman& pac)
{
    // Replan path every GHOST_REPLAN_PERIOD ticks
    if (replanTick % GHOST_REPLAN_PERIOD == 0 || cachedPath.size() <= 1) {
        cachedPath = astarPath(g, row, col, pac.getRow(), pac.getCol());
    }
    ++replanTick;

    // Move only every GHOST_STEP_PERIOD ticks
    if (stepTick % GHOST_STEP_PERIOD == 0) {
        if (cachedPath.size() > 1) {
            row = cachedPath[1].r;
            col = cachedPath[1].c;
            cachedPath.erase(cachedPath.begin());
        }
    }
    ++stepTick;
}

void Ghost::draw() const
{
    const float cx = (float)col + 0.5f;
    const float cy = (float)row + 0.5f;
    const float r = 0.42f;

    glColor3f(1.0f, 0.2f, 0.2f);
    glBegin(GL_POLYGON);
    for (float a = 0.0f; a <= (float)M_PI; a += 0.06f)
        glVertex2f(cx + r * cosf(a), cy + r * sinf(a));
    glVertex2f(cx + r, cy - r);
    glVertex2f(cx - r, cy - r);
    glEnd();

    // eyes
    glColor3f(1, 1, 1);
    const float eyeR = 0.09f, eyeDX = 0.15f, eyeY = cy + 0.15f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx - eyeDX, eyeY);
    for (int i = 0;i <= 40;++i) { float t = 2.0f * (float)M_PI * i / 40; glVertex2f((cx - eyeDX) + eyeR * cosf(t), eyeY + eyeR * sinf(t)); }
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx + eyeDX, eyeY);
    for (int i = 0;i <= 40;++i) { float t = 2.0f * (float)M_PI * i / 40; glVertex2f((cx + eyeDX) + eyeR * cosf(t), eyeY + eyeR * sinf(t)); }
    glEnd();

    glColor3f(0.0f, 0.0f, 0.7f);
    const float pupilR = 0.045f, py = -0.03f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx - eyeDX, eyeY + py);
    for (int i = 0;i <= 30;++i) { float t = 2.0f * (float)M_PI * i / 30; glVertex2f((cx - eyeDX) + pupilR * cosf(t), (eyeY + py) + pupilR * sinf(t)); }
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx + eyeDX, eyeY + py);
    for (int i = 0;i <= 30;++i) { float t = 2.0f * (float)M_PI * i / 30; glVertex2f((cx + eyeDX) + pupilR * cosf(t), (eyeY + py) + pupilR * sinf(t)); }
    glEnd();
}
