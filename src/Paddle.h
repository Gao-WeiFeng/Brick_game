#ifndef PADDLE_H
#define PADDLE_H
#include "raylib.h"
class Paddle {
private:
    Rectangle rect;
    float screenWidth;
    float originalWidth; // 记录原始宽度，用来恢复
    float extendEffectTime; // 加长效果的剩余时间
public:
    Paddle(float x, float y, float width, float height);
    void MoveLeft(float speed);
    void MoveRight(float speed);
    void Draw();
    Rectangle GetRect() { return rect; }
    
    // 新加的方法：加长板，还有更新效果时间
    void Extend(float extraWidth, float duration);
    void Update(float dt);
    float GetOriginalWidth() { return originalWidth; }
};
#endif