#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include <vector>
#include <string>

// 游戏状态枚举，代替原来的一堆bool变量
enum class GameState {
    MENU,       // 主菜单
    PLAYING,    // 游戏中
    PAUSED,     // 暂停
    GAMEOVER,   // 游戏结束（失败）
    VICTORY,    // 胜利
    LEADERBOARD // 排行榜
};

// 排行榜条目结构
struct ScoreEntry {
    char name[32];
    int score;
    time_t timestamp;
};

// 排行榜类
class Leaderboard {
private:
    static const int MAX_ENTRIES = 10;
    ScoreEntry entries[MAX_ENTRIES];
    int count;
    const char* filename;
public:
    Leaderboard(const char* file);
    void Load();
    void Save();
    int AddScore(const char* name, int score);
    bool GetEntry(int rank, ScoreEntry& entry);
    int GetCount();
    bool CanEnter(int score);
};

class Game {
private:
    // 游戏核心对象
    Ball ball;
    Paddle paddle;
    std::vector<Brick> bricks;
    Leaderboard leaderboard;
    
    // 游戏状态
    GameState currentState;
    int score;
    int lives;
    int winCount;
    int playerRank;
    float gameTime;
    
    // 字体相关
    Font chineseFont;
    bool fontLoaded;
    
    // 配置参数（后面会从配置文件读取）
    int screenWidth;
    int screenHeight;
    std::string windowTitle;
    
    // 球的配置
    float ballRadius;
    float ballGravity;
    float ballMaxSpeed;
    float ballBounceForce;
    
    // 板的配置
    float paddleWidth;
    float paddleHeight;
    float paddleSpeed;
    float paddleBoostSpeed;
    
    // 砖块的配置
    int brickRows;
    int brickCols;
    float brickWidth;
    float brickHeight;
    
    // 游戏的配置
    int initialLives;
    int scorePerBrick;
    float timeMultiplierDecay;

    // 私有辅助方法
    void InitChineseFont();
    void DrawChineseText(const char* text, int x, int y, int fontSize, Color color);
    void DrawChineseTextCentered(const char* text, int y, int fontSize, Color color);
    int CalculateScore(int baseScore);
    void ResetGame();

public:
    // 构造函数
    Game();
    
    // 游戏的核心流程方法
    void Init();
    void Update();
    void Draw();
    void Shutdown();
    
    // 加载配置文件（后面会用到）
    void LoadConfig(const std::string& path);
};

#endif