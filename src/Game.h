#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Ball.h"
#include "Brick.h"
#include "Paddle.h"
#include "Leaderboard.h" // 你原来的排行榜类，保留！
#include <vector>

// 游戏状态机（按PPT要求）
enum class GameState {
    MENU,
    PLAYING,
    GAME_OVER,
    VICTORY,
    LEADERBOARD
};

class Game {
private:
    int screenWidth;
    int screenHeight;
    const char* title;

    GameState currentState;

    Ball ball;
    std::vector<Brick> bricks;
    Paddle paddle;
    Leaderboard leaderboard; // 保留你原来的排行榜！

    int score;
    int lives;
    bool paused;

    // 配置参数（从config.json读）
    float ballGravity;
    float ballMaxSpeed;
    float paddleSpeed;
    int initialLives;
    int scorePerBrick;

    void LoadConfig(); // 加载配置文件

public:
    Game();
    void Init();
    void Update();
    void Draw();
    void Reset();
};

#endif