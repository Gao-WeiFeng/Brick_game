#ifndef BRICK_H
#define BRICK_H
#include "raylib.h"
class Brick {
private:
    Rectangle rect;
    bool active;
    Color color;
public:
    Brick(float x, float y, float w, float h, Color c);
    void Draw();
    Rectangle GetRect() { return rect; }
    bool IsActive() { return active; }
    void SetActive(bool a) { active = a; }
    Color GetColor() { return color; } // 新加的获取颜色的方法
};
#endif