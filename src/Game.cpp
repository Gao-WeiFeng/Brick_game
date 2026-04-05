#include "Game.h"
#include "json.hpp"
#include <fstream>
using json = nlohmann::json;

// 你原来的中文初始化函数，保留！
extern void InitChineseFont();

Game::Game()
    : leaderboard("scores.txt"), // 你原来的排行榜初始化
      screenWidth(800), screenHeight(600), title("Breakout"),
      ball({400.0f, 500.0f}, {0.0f, 0.0f}, 10.0f),
      paddle(300.0f, 550.0f, 120.0f, 15.0f),
      score(0), lives(3), paused(false),
      currentState(GameState::MENU)
{
    // 默认配置
    ballGravity = 0.08f;
    ballMaxSpeed = 15.0f;
    paddleSpeed = 6.0f;
    initialLives = 3;
    scorePerBrick = 10;
}

// 加载config.json配置（按PPT要求）
void Game::LoadConfig() {
    try {
        std::ifstream f("config.json");
        json config = json::parse(f);

        // 读取配置
        screenWidth = config["window"]["width"];
        screenHeight = config["window"]["height"];
        title = config["window"]["title"].c_str();

        ballGravity = config["ball"]["gravity"];
        ballMaxSpeed = config["ball"]["maxSpeed"];
        paddleSpeed = config["paddle"]["speed"];

        initialLives = config["game"]["initialLives"];
        scorePerBrick = config["game"]["scorePerBrick"];
    } catch (...) {
        // 配置读失败就用默认值，不影响运行
    }
}

void Game::Init() {
    LoadConfig(); // 先加载配置
    InitChineseFont(); // 你原来的中文初始化，保留！

    // 初始化砖块
    bricks.clear();
    Color colors[] = {RED, ORANGE, YELLOW, GREEN, BLUE};
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 8; col++) {
            bricks.emplace_back(
                50 + col * 95,
                80 + row * 35,
                85,
                25,
                colors[row]
            );
        }
    }

    // 重置游戏
    score = 0;
    lives = initialLives;
    ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width/2, paddle.GetRect().y);
}

void Game::Reset() {
    score = 0;
    lives = initialLives;
    Init();
    currentState = GameState::PLAYING;
}

void Game::Update() {
    // 菜单状态输入
    if (currentState == GameState::MENU) {
        if (IsKeyPressed(KEY_ENTER)) {
            currentState = GameState::PLAYING;
        }
        if (IsKeyPressed(KEY_L)) {
            currentState = GameState::LEADERBOARD;
        }
        return;
    }

    // 排行榜状态
    if (currentState == GameState::LEADERBOARD) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            currentState = GameState::MENU;
        }
        return;
    }

    // 游戏结束/胜利状态
    if (currentState == GameState::GAME_OVER || currentState == GameState::VICTORY) {
        if (IsKeyPressed(KEY_R)) Reset();
        return;
    }

    // 游戏中输入
    if (IsKeyPressed(KEY_SPACE) && !ball.IsLaunched()) {
        ball.Launch();
    }
    if (IsKeyPressed(KEY_P)) paused = !paused;
    if (IsKeyPressed(KEY_L)) currentState = GameState::LEADERBOARD;

    if (paused) return;

    // 物理更新（你原来的逻辑，全保留！）
    ball.ApplyGravity();
    ball.Move();
    ball.BounceEdge();

    // 挡板移动
    if (IsKeyDown(KEY_LEFT)) paddle.Move(-paddleSpeed);
    if (IsKeyDown(KEY_RIGHT)) paddle.Move(paddleSpeed);

    // 挡板碰撞
    if (ball.CheckPaddleCollision(paddle.GetRect())) {
        ball.AddBounceForce();
    }

    // 砖块碰撞
    for (auto& brick : bricks) {
        if (brick.IsActive() && ball.CheckBrickCollision(brick.GetRect())) {
            brick.SetActive(false);
            score += scorePerBrick;
            ball.AddBounceForce();
        }
    }

    // 掉落生命
    if (ball.GetPosition().y > screenHeight) {
        lives--;
        ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width/2, paddle.GetRect().y);
        if (lives <= 0) {
            currentState = GameState::GAME_OVER;
            leaderboard.AddScore(score);
        }
    }

    // 胜利
    bool allBricksDestroyed = true;
    for (auto& brick : bricks) {
        if (brick.IsActive()) {
            allBricksDestroyed = false;
            break;
        }
    }
    if (allBricksDestroyed) {
        currentState = GameState::VICTORY;
        leaderboard.AddScore(score);
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground({30,30,40,255});

    // 菜单
    if (currentState == GameState::MENU) {
        DrawText("BREAKOUT", 300, 150, 50, WHITE);
        DrawText("按 ENTER 开始游戏", 280, 280, 25, WHITE);
        DrawText("按 L 查看排行榜", 290, 320, 20, GRAY);
        EndDrawing();
        return;
    }

    // 排行榜
    if (currentState == GameState::LEADERBOARD) {
        DrawText("排行榜", 350, 100, 30, WHITE);
        auto scores = leaderboard.GetScores();
        for (int i=0; i<scores.size() && i<10; i++) {
            DrawText(TextFormat("%d. %d", i+1, scores[i]), 350, 150+i*30, 20, WHITE);
        }
        DrawText("按 ESC 返回", 320, 500, 20, GRAY);
        EndDrawing();
        return;
    }

    // 游戏中
    if (currentState == GameState::PLAYING) {
        ball.Draw();
        paddle.Draw();
        for (auto& brick : bricks) {
            if (brick.IsActive()) brick.Draw();
        }

        // UI
        DrawText(TextFormat("分数: %06d", score), 10, 10, 20, WHITE);
        DrawText(TextFormat("生命: %d", lives), 680, 10, 20, WHITE);
        if (paused) DrawText("暂停", 370, 280, 40, WHITE);
    }

    // 游戏结束
    if (currentState == GameState::GAME_OVER) {
        DrawText("游戏结束", 280, 250, 50, RED);
        DrawText("按 R 重新开始", 300, 320, 20, WHITE);
    }

    // 胜利
    if (currentState == GameState::VICTORY) {
        DrawText("你赢了!", 320, 250, 50, GREEN);
        DrawText("按 R 重新开始", 300, 320, 20, WHITE);
    }

    EndDrawing();
}