#ifndef POWERUP_H
#define POWERUP_H
#include "raylib.h"
#include <memory>

// 道具类型枚举，对应我们的三个道具
enum class PowerUpType {
    PADDLE_EXTEND, // 加长板
    MULTI_BALL,    // 多球
    SLOW_BALL      // 减速球
};

// 粒子特效的结构体
struct Particle {
    Vector2 pos;
    Vector2 vel;
    Color color;
    float life;
    float maxLife;
};

// 道具效果的基类，工厂模式的核心
class Game; // 前向声明，避免循环引用
class PowerUpEffect {
public:
    virtual void Apply(Game& game) = 0;
    virtual ~PowerUpEffect() = default;
};

// 加长板的效果类
class ExtendPaddleEffect : public PowerUpEffect {
private:
    float extraWidth;
    float duration;
public:
    ExtendPaddleEffect(float w, float d) : extraWidth(w), duration(d) {}
    void Apply(Game& game) override;
};

// 多球的效果类
class MultiBallEffect : public PowerUpEffect {
private:
    int extraBalls;
public:
    MultiBallEffect(int count) : extraBalls(count) {}
    void Apply(Game& game) override;
};

// 减速球的效果类
class SlowBallEffect : public PowerUpEffect {
private:
    float duration;
public:
    SlowBallEffect(float d) : duration(d) {}
    void Apply(Game& game) override;
};

// 道具本身的类，用来处理掉落、碰撞
class PowerUp {
public:
    Vector2 position;
    PowerUpType type;
    bool active;
    
    PowerUp(float x, float y, PowerUpType t);
    void Update(float dt);
    void Draw();
    Rectangle GetRect();
};

// 工厂函数：根据类型创建对应的效果，避免if-else
std::unique_ptr<PowerUpEffect> CreatePowerUp(PowerUpType type, 
    float extra_width, float extra_balls, float speed_factor, float duration);

#endif