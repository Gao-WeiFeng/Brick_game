#include "Game.h"
#include "json.hpp"
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <algorithm>

using json = nlohmann::json;

// Leaderboard的实现
Leaderboard::Leaderboard(const char* file) : count(0), filename(file) { Load(); }

void Leaderboard::Load() {
    FILE* f = fopen(filename, "r");
    if (f) {
        count = 0;
        while (count < MAX_ENTRIES && fscanf(f, "%31s %d %ld", entries[count].name, &entries[count].score, &entries[count].timestamp) == 3) {
            count++;
        }
        fclose(f);
    }
}

void Leaderboard::Save() {
    FILE* f = fopen(filename, "w");
    if (f) {
        for (int i = 0; i < count; i++) fprintf(f, "%s %d %ld\n", entries[i].name, entries[i].score, entries[i].timestamp);
        fclose(f);
    }
}

int Leaderboard::AddScore(const char* name, int score) {
    if (count >= MAX_ENTRIES && score <= entries[count - 1].score) return 0;
    ScoreEntry newEntry;
    strncpy(newEntry.name, name, 31); newEntry.name[31] = '\0';
    newEntry.score = score; newEntry.timestamp = time(nullptr);
    int pos = 0;
    while (pos < count && entries[pos].score >= score) pos++;
    if (count < MAX_ENTRIES) count++;
    for (int i = count - 1; i > pos; i--) entries[i] = entries[i - 1];
    entries[pos] = newEntry;
    Save();
    return pos + 1;
}

bool Leaderboard::GetEntry(int rank, ScoreEntry& entry) { 
    if (rank > 0 && rank <= count) { entry = entries[rank - 1]; return true; } 
    return false; 
}

int Leaderboard::GetCount() { return count; }
bool Leaderboard::CanEnter(int score) { return count < MAX_ENTRIES || score > entries[count - 1].score; }

// Game类的实现
Game::Game() : 
    ball({400.0f, 530.0f}, {0.0f, 0.0f}, 10.0f),
    paddle(340.0f, 550.0f, 120.0f, 15.0f),
    leaderboard("scores.txt"),
    currentState(GameState::MENU)
{
    // 先给默认值，后面会从配置文件覆盖
    screenWidth = 800;
    screenHeight = 600;
    windowTitle = "Breakout - Enhanced Edition";
    
    ballRadius = 10.0f;
    ballGravity = 0.08f;
    ballMaxSpeed = 15.0f;
    ballBounceForce = 0.5f;
    
    paddleWidth = 120.0f;
    paddleHeight = 15.0f;
    paddleSpeed = 18.0f;
    paddleBoostSpeed = 28.0f;
    
    brickRows = 5;
    brickCols = 8;
    brickWidth = 85.0f;
    brickHeight = 25.0f;
    
    initialLives = 3;
    scorePerBrick = 10;
    timeMultiplierDecay = 0.05f;
}

void Game::InitChineseFont() {
    const char* text = "分数生命暂停继续重新开始游戏结束胜利排行榜第名按P暂停按R重新开始时间倍率落地惩罚恭喜进入空格发射等待";
    int codepointCount = 0;
    int* codepoints = LoadCodepoints(text, &codepointCount);
    const char* fontPaths[] = { "../fonts/NotoSansSC.otf", "/home/cly/Test/Breakout/fonts/NotoSansSC.otf", "/usr/share/fonts/truetype/wqy/wqy-microhei.ttc", "/mnt/c/Windows/Fonts/msyh.ttc" };
    printf("正在尝试加载中文字体...\n");
    fontLoaded = false;
    for (int i = 0; i < 4; i++) {
        if (FileExists(fontPaths[i])) {
            printf("  找到字体文件: %s\n", fontPaths[i]);
            chineseFont = LoadFontEx(fontPaths[i], 48, codepoints, codepointCount);
            if (chineseFont.texture.id != 0) { 
                printf("  字体加载成功！\n"); 
                fontLoaded = true; 
                break; 
            }
        }
    }
    if (!fontLoaded) { 
        printf("警告: 字体加载失败，使用默认字体\n"); 
        chineseFont = GetFontDefault(); 
    }
    UnloadCodepoints(codepoints);
}

void Game::DrawChineseText(const char* text, int x, int y, int fontSize, Color color) { 
    Vector2 pos = { (float)x, (float)y }; 
    DrawTextEx(chineseFont, text, pos, (float)fontSize, 2, color); 
}

void Game::DrawChineseTextCentered(const char* text, int y, int fontSize, Color color) { 
    Vector2 size = MeasureTextEx(chineseFont, text, (float)fontSize, 2); 
    DrawChineseText(text, (screenWidth - (int)size.x) / 2, y, fontSize, color); 
}

int Game::CalculateScore(int baseScore) {
    float multiplier = 5.0f - gameTime * timeMultiplierDecay;
    if (multiplier < 1.0f) multiplier = 1.0f;
    return (int)(baseScore * multiplier);
}

void Game::ResetGame() {
    // 重置游戏状态
    ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width / 2, paddle.GetRect().y);
    score = 0; 
    lives = initialLives; 
    gameTime = 0.0f;
    playerRank = 0;
    
    // 重新创建砖块
    bricks.clear();
    Color brickColors[] = {RED, ORANGE, YELLOW, GREEN, BLUE};
    for (int row = 0; row < brickRows; row++) {
        for (int col = 0; col < brickCols; col++) {
            bricks.emplace_back(50 + col * brickWidth, 80 + row * brickHeight, brickWidth, brickHeight, brickColors[row]);
        }
    }
    winCount = (int)bricks.size();
    
    currentState = GameState::PLAYING;
}

void Game::LoadConfig(const std::string& path) {
    try {
        std::ifstream f(path);
        if (!f.is_open()) {
            printf("警告：无法打开配置文件，使用默认参数\n");
            return;
        }
        json config = json::parse(f);
        
        // 读取窗口配置
        if (config.contains("window")) {
            screenWidth = config["window"]["width"];
            screenHeight = config["window"]["height"];
            windowTitle = config["window"]["title"];
        }
        
        // 读取球的配置
        if (config.contains("ball")) {
            ballRadius = config["ball"]["radius"];
            ballGravity = config["ball"]["gravity"];
            ballMaxSpeed = config["ball"]["maxSpeed"];
            ballBounceForce = config["ball"]["bounceForce"];
        }
        
        // 读取板的配置
        if (config.contains("paddle")) {
            paddleWidth = config["paddle"]["width"];
            paddleHeight = config["paddle"]["height"];
            paddleSpeed = config["paddle"]["speed"];
            paddleBoostSpeed = config["paddle"]["boostSpeed"];
        }
        
        // 读取砖块的配置
        if (config.contains("bricks")) {
            brickRows = config["bricks"]["rows"];
            brickCols = config["bricks"]["cols"];
            brickWidth = config["bricks"]["width"];
            brickHeight = config["bricks"]["height"];
        }
        
        // 读取游戏的配置
        if (config.contains("game")) {
            initialLives = config["game"]["initialLives"];
            scorePerBrick = config["game"]["scorePerBrick"];
            timeMultiplierDecay = config["game"]["timeMultiplierDecay"];
        }
        
        printf("配置文件加载成功！\n");
    } catch (std::exception& e) {
        printf("配置文件解析失败：%s，使用默认参数\n", e.what());
    }
}

void Game::Init() {
    // 初始化字体
    InitChineseFont();
    
    // 加载配置文件
    LoadConfig("../config.json");
    
    // 重新初始化对象，用配置的参数
    ball = Ball({(float)screenWidth/2, (float)screenHeight - 70}, {0.0f, 0.0f}, ballRadius);
    paddle = Paddle((screenWidth - paddleWidth)/2, screenHeight - 50, paddleWidth, paddleHeight);
    
    // 初始化游戏
    ResetGame();
    
    SetTargetFPS(60);
}

void Game::Update() {
    // 处理全局按键
    if (IsKeyPressed(KEY_R)) {
        // 按R重置游戏，不管什么状态
        ResetGame();
        return;
    }
    
    if (IsKeyPressed(KEY_L)) {
        // 按L切换排行榜
        if (currentState == GameState::LEADERBOARD) {
            currentState = GameState::MENU;
        } else {
            currentState = GameState::LEADERBOARD;
        }
        return;
    }

    // 根据不同状态处理不同的逻辑
    switch(currentState) {
        case GameState::MENU:
            // 菜单状态，按空格开始游戏
            if (IsKeyPressed(KEY_SPACE)) {
                currentState = GameState::PLAYING;
            }
            break;
            
        case GameState::PLAYING:
        {
            // 游戏中状态
            if (IsKeyPressed(KEY_P)) {
                // 按P暂停
                currentState = GameState::PAUSED;
                return;
            }
            
            // 处理输入
            float currentSpeed = (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) ? paddleBoostSpeed : paddleSpeed;
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) paddle.MoveLeft(currentSpeed);
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) paddle.MoveRight(currentSpeed);
            
            // 球的逻辑
            if (!ball.IsLaunched()) {
                ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width / 2, paddle.GetRect().y);
                if (IsKeyPressed(KEY_SPACE)) ball.Launch(paddle.GetRect().x + paddle.GetRect().width / 2, paddle.GetRect().width);
            }
            
            if (ball.IsLaunched()) gameTime += GetFrameTime();
            
            ball.ApplyGravity(); 
            ball.Move(); 
            ball.BounceEdge(screenWidth, screenHeight); 
            ball.BouncePaddle(paddle.GetRect());
            
            // 砖块碰撞
            for (auto& brick : bricks) {
                if (brick.IsActive() && ball.CheckBrickCollision(brick.GetRect())) {
                    brick.SetActive(false); 
                    score += CalculateScore(scorePerBrick); 
                    winCount--; 
                    break;
                }
            }
            
            // 胜利检测
            if (winCount <= 0) { 
                currentState = GameState::VICTORY; 
                if (leaderboard.CanEnter(score)) playerRank = leaderboard.AddScore("Player", score); 
            }
            
            // 掉出屏幕，生命减少
            if (ball.GetPosition().y > screenHeight + 50) {
                lives--; 
                score -= 50; 
                if (score < 0) score = 0;
                if (lives <= 0) { 
                    currentState = GameState::GAMEOVER; 
                    if (leaderboard.CanEnter(score)) playerRank = leaderboard.AddScore("Player", score); 
                }
                else ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width / 2, paddle.GetRect().y);
            }
        }
            break;
            
        case GameState::PAUSED:
            // 暂停状态，按P继续
            if (IsKeyPressed(KEY_P)) {
                currentState = GameState::PLAYING;
            }
            break;
            
        case GameState::GAMEOVER:
        case GameState::VICTORY:
            // 结束状态，按R重置已经在全局处理了
            break;
            
        case GameState::LEADERBOARD:
            // 排行榜状态，按L关闭已经处理了
            break;
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(Color{30, 30, 40, 255});
    
    // 绘制边界
    DrawRectangle(0, 0, 5, screenHeight, GRAY); 
    DrawRectangle(screenWidth - 5, 0, 5, screenHeight, GRAY); 
    DrawRectangle(0, 0, screenWidth, 5, GRAY);

    // 根据状态绘制不同的内容
    switch(currentState) {
        case GameState::MENU:
            // 主菜单
            DrawChineseTextCentered("打砖块游戏", screenHeight/2 - 80, 48, YELLOW);
            DrawChineseTextCentered("按 空格 开始游戏", screenHeight/2, 24, WHITE);
            DrawChineseTextCentered("按 L 查看排行榜", screenHeight/2 + 40, 20, Fade(WHITE, 0.6f));
            break;
            
        case GameState::PLAYING:
        case GameState::PAUSED:
        {
            // 游戏中或者暂停，先绘制游戏元素
            for (auto& brick : bricks) brick.Draw();
            paddle.Draw(); 
            ball.Draw();
            
            // 绘制UI
            DrawChineseText("分数:", 20, 8, 24, WHITE); 
            DrawText(TextFormat("%d", score), 80, 10, 24, YELLOW);
            DrawChineseText("生命:", 650, 8, 24, WHITE); 
            DrawText(TextFormat("%d", lives), 710, 10, 24, lives > 1 ? GREEN : RED);
            DrawChineseText("时间:", 20, 35, 20, Fade(WHITE, 0.8f)); 
            DrawText(TextFormat("%.1f秒", gameTime), 75, 37, 20, Fade(WHITE, 0.8f));
            float currentMultiplier = 5.0f - gameTime * timeMultiplierDecay; 
            if (currentMultiplier < 1.0f) currentMultiplier = 1.0f;
            DrawText(TextFormat("x%.1f", currentMultiplier), 140, 37, 20, currentMultiplier > 2.0f ? GREEN : Fade(WHITE, 0.5f));
            
            if (!ball.IsLaunched()) DrawChineseTextCentered("按空格发射", 55, 20, YELLOW);
            DrawChineseText("P-暂停 R-重开 L-排行", 280, 12, 18, Fade(WHITE, 0.6f));
            
            if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) DrawText(">>> BOOST <<<", 350, 575, 18, YELLOW);
            
            // 如果是暂停，覆盖暂停界面
            if (currentState == GameState::PAUSED) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
                DrawChineseTextCentered("暂停", screenHeight/2 - 40, 48, YELLOW); 
                DrawChineseTextCentered("按 P 继续", screenHeight/2 + 30, 24, WHITE);
            }
        }
            break;
            
        case GameState::GAMEOVER:
        case GameState::VICTORY:
            // 游戏结束或者胜利
            for (auto& brick : bricks) brick.Draw();
            paddle.Draw(); 
            ball.Draw();
            
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.85f));
            if (currentState == GameState::VICTORY) { 
                DrawChineseTextCentered("胜利!", screenHeight/2 - 80, 48, GREEN); 
                DrawText(TextFormat("FINAL SCORE: %d", score), screenWidth/2 - 100, screenHeight/2 - 30, 28, YELLOW); 
            }
            else { 
                DrawChineseTextCentered("游戏结束", screenHeight/2 - 80, 48, RED); 
                DrawText(TextFormat("SCORE: %d", score), screenWidth/2 - 60, screenHeight/2 - 30, 28, YELLOW); 
            }
            if (playerRank > 0) { 
                char rankText[64]; 
                sprintf(rankText, "恭喜进入排行榜第%d名!", playerRank); 
                DrawChineseTextCentered(rankText, screenHeight/2 + 40, 24, GOLD); 
            }
            DrawChineseTextCentered("按 R 重新开始", screenHeight/2 + 90, 24, WHITE);
            break;
            
        case GameState::LEADERBOARD:
            // 排行榜
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.9f)); 
            DrawChineseTextCentered("排行榜", 40, 36, GOLD);
            DrawText("RANK", 150, 90, 20, Fade(WHITE, 0.6f)); 
            DrawText("NAME", 250, 90, 20, Fade(WHITE, 0.6f)); 
            DrawText("SCORE", 450, 90, 20, Fade(WHITE, 0.6f)); 
            DrawText("DATE", 550, 90, 20, Fade(WHITE, 0.6f));
            DrawLine(150, 115, 700, 115, Fade(WHITE, 0.3f));
            for (int i = 0; i < leaderboard.GetCount() && i < 10; i++) {
                ScoreEntry entry; 
                if (leaderboard.GetEntry(i + 1, entry)) {
                    int y = 130 + i * 35; 
                    Color rowColor = (i == 0) ? GOLD : (i == 1) ? Color{192,192,192,255} : (i == 2) ? Color{205,127,50,255} : WHITE;
                    DrawText(TextFormat("#%d", i + 1), 150, y, 22, rowColor); 
                    DrawText(entry.name, 250, y, 22, rowColor); 
                    DrawText(TextFormat("%d", entry.score), 450, y, 22, rowColor);
                    char dateStr[32]; 
                    strftime(dateStr, sizeof(dateStr), "%m/%d", localtime(&entry.timestamp)); 
                    DrawText(dateStr, 550, y, 20, Fade(rowColor, 0.7f));
                }
            }
            if (leaderboard.GetCount() == 0) DrawChineseTextCentered("暂无记录", screenHeight/2, 24, Fade(WHITE, 0.5f));
            DrawChineseTextCentered("按 L 关闭排行榜", screenHeight - 50, 20, Fade(WHITE, 0.5f));
            break;
    }

    EndDrawing();
}

void Game::Shutdown() {
    // 释放资源
    UnloadFont(chineseFont);
}