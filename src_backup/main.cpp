#include "raylib.h"
#include "Game.h"

int main() {
    // 初始化窗口（大小会在Game里根据配置更新）
    InitWindow(800, 600, "Breakout");
    
    // 创建游戏对象
    Game game;
    game.Init();
    
    // 主循环
    while (!WindowShouldClose()) {
        game.Update();
        game.Draw();
    }
    
    // 清理
    game.Shutdown();
    CloseWindow();
    return 0;
}