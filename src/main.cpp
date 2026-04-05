#include "raylib.h"
#include "Game.h"
#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"

int main() {
    const int width = 800, height = 600;
    InitWindow(width, height, "Breakout");
    SetTargetFPS(60);

    Game game;
    game.Init();

    while (!WindowShouldClose()) {
        game.Update();
        game.Draw();
    }

    CloseWindow();
    return 0;
}