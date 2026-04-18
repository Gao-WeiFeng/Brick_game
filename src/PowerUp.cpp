#include "PowerUp.h"
#include "Game.h"
#include "Ball.h"
#include "Paddle.h"
#include <cstdlib>
#include <algorithm>
#include <cmath>

// 各个效果的实现
void ExtendPaddleEffect::Apply(Game& game) {
    game.GetPaddle().Extend(extraWidth, duration);
}

void MultiBallEffect::Apply(Game& game) {
    // 给每个现有的球，生成新的球
    auto& balls = game.GetBalls();
    int originalCount = balls.size();
    for (int i = 0; i < originalCount && balls.size() < originalCount + extraBalls; i++) {
        auto& oldBall = balls[i];
        if (oldBall.IsLaunched()) {
            // 随机方向的新球
            float angle = (float)((rand() % 120 + 30) * 3.14159f / 180.0f);
            if (rand() % 2 == 0) angle = 3.14159f - angle;
            float speed = 8.0f;
            Vector2 newSpeed = {
                speed * cosf(angle),
                -speed * abs(sinf(angle))
            };
            Ball newBall = oldBall;
            newBall.Reset(oldBall.GetPosition(), newSpeed);
            balls.push_back(newBall);
        }
    }
}

void SlowBallEffect::Apply(Game& game) {
    game.ActivateSlowEffect(duration);
}

PowerUp::PowerUp(float x, float y, PowerUpType t) : position({x, y}), type(t), active(true) {}

Rectangle PowerUp::GetRect() {
    return {position.x, position.y, 30, 30};
}

void PowerUp::Update(float dt) {
    // 道具向下掉落，速度100像素每秒
    position.y += 100 * dt;
}

void PowerUp::Draw() {
    Rectangle rect = GetRect();
    Color color;
    char text[2];
    // 不同道具不同颜色和标识
    switch(type) {
        case PowerUpType::PADDLE_EXTEND:
            color = YELLOW;
            text[0] = 'E'; text[1] = '\0';
            break;
        case PowerUpType::MULTI_BALL:
            color = GREEN;
            text[0] = 'M'; text[1] = '\0';
            break;
        case PowerUpType::SLOW_BALL:
            color = BLUE;
            text[0] = 'S'; text[1] = '\0';
            break;
        default:
            color = WHITE;
            text[0] = '?'; text[1] = '\0';
    }
    // 绘制道具，加个光晕效果
    DrawRectangleRec(rect, color);
    DrawRectangleLinesEx(rect, 2, WHITE);
    DrawText(text, rect.x + 8, rect.y + 5, 20, BLACK);
    DrawCircleGradient(rect.x + 15, rect.y + 15, 20, Fade(color, 0.2f), Fade(color, 0));
}

// 工厂函数实现
std::unique_ptr<PowerUpEffect> CreatePowerUp(PowerUpType type, 
    float extra_width, float extra_balls, float speed_factor, float duration) {
    switch(type) {
        case PowerUpType::PADDLE_EXTEND:
            return std::make_unique<ExtendPaddleEffect>(extra_width, duration);
        case PowerUpType::MULTI_BALL:
            return std::make_unique<MultiBallEffect>((int)extra_balls);
        case PowerUpType::SLOW_BALL:
            return std::make_unique<SlowBallEffect>(duration);
        default:
            return nullptr;
    }
}