#include <iostream>
#include "Ball.h"
#include "Brick.h"
#include "raylib.h"

// 让窗口停留
#include <cstdlib>
void pauseConsole() {
    system("pause");
}

// 测试1：小球创建
void TestBallCreation() {
    Ball ball({400.0f, 500.0f}, {0.0f, 0.0f}, 10.0f);
    std::cout << "TestBallCreation: PASS ✅\n";
}

// 测试2：砖块创建
void TestBrickCreation() {
    Brick brick(50, 80, 85, 25, RED);
    std::cout << "TestBrickCreation: PASS ✅\n";
}

int main() {
    std::cout << "=== 单元测试开始 ===\n";
    TestBallCreation();
    TestBrickCreation();
    std::cout << "=== 所有测试通过！===\n\n";

    // 让窗口停在这里！
    pauseConsole();
    return 0;
}