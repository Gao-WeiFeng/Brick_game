#include "Paddle.h"
Paddle::Paddle(float x, float y, float width, float height) {
    rect = { x, y, width, height };
    screenWidth = 800;
    originalWidth = width; // 初始化原始宽度
    extendEffectTime = 0.0f; // 初始没有效果
}
void Paddle::MoveLeft(float speed) {
    rect.x -= speed;
    if (rect.x < 5) rect.x = 5;
}
void Paddle::MoveRight(float speed) {
    rect.x += speed;
    if (rect.x + rect.width > screenWidth - 5) rect.x = screenWidth - rect.width - 5;
}
// 更新效果时间，到期自动恢复宽度
void Paddle::Update(float dt) {
    if (extendEffectTime > 0) {
        extendEffectTime -= dt;
        // 效果到期，恢复原始宽度
        if (extendEffectTime <= 0) {
            rect.width = originalWidth;
            extendEffectTime = 0.0f;
        }
    }
}
// 加长板的方法，重置效果时间，避免无限变长
void Paddle::Extend(float extraWidth, float duration) {
    rect.width = originalWidth + extraWidth;
    extendEffectTime = duration;
}
void Paddle::Draw() {
    Color color = BLUE;
    // 有加长效果的时候，板变成黄色，提示你
    if (extendEffectTime > 0) {
        color = YELLOW;
    }
    DrawRectangleRec(rect, color);
    DrawRectangleLinesEx(rect, 2, SKYBLUE);
}