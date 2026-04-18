#include <iostream>
#include <cassert>
#include "Ball.h"
#include "Brick.h"
#include "Paddle.h"
#include "raylib.h"

// 让测试窗口停留，方便你看结果
#include <cstdlib>
void pauseConsole() {
    system("pause");
}

// 测试1：小球创建是否正常
void TestBallCreation() {
    Ball ball({400.0f, 500.0f}, {0.0f, 0.0f}, 10.0f);
    // 检查初始位置和半径是不是正确
    assert(ball.GetPosition().x == 400.0f);
    assert(ball.GetPosition().y == 500.0f);
    assert(ball.GetRadius() == 10.0f);
    std::cout << "TestBallCreation: PASS ✅\n";
}

// 测试2：砖块创建是否正常
void TestBrickCreation() {
    Brick brick(50, 80, 85, 25, RED);
    // 检查砖块的位置和初始状态
    assert(brick.GetRect().x == 50);
    assert(brick.GetRect().y == 80);
    assert(brick.IsActive() == true);
    std::cout << "TestBrickCreation: PASS ✅\n";
}

// 测试3：球和砖块的正常碰撞（作业要求的第一个测试用例）
void TestBrickCollisionNormal() {
    // 创建一个球，位置在砖块的正上方，准备向下撞砖块
    Ball ball({100.0f, 70.0f}, {0.0f, 2.0f}, 10.0f);
    // 创建一个测试用的砖块
    Brick brick(50, 80, 85, 25, RED);
    
    // 记录碰撞前的速度
    float oldSpeedY = ball.GetSpeed().y;
    
    // 执行碰撞检测
    bool collided = ball.CheckBrickCollision(brick.GetRect());
    
    // 断言：应该检测到碰撞
    assert(collided == true);
    // 断言：碰撞之后，球的y方向速度应该反转（变成向上）
    assert(ball.GetSpeed().y == -oldSpeedY);
    std::cout << "TestBrickCollisionNormal: PASS ✅\n";
}

// 测试4：球和砖块的边缘碰撞（作业要求的第二个测试用例）
void TestBrickCollisionEdge() {
    // 创建一个球，位置在砖块的右侧边缘，准备向左撞砖块的边
    Ball ball({145.0f, 90.0f}, {-2.0f, 0.0f}, 10.0f);
    Brick brick(50, 80, 85, 25, RED);
    
    // 记录碰撞前的x方向速度
    float oldSpeedX = ball.GetSpeed().x;
    
    bool collided = ball.CheckBrickCollision(brick.GetRect());
    
    // 断言：边缘也能检测到碰撞
    assert(collided == true);
    // 断言：x方向速度反转（变成向右）
    assert(ball.GetSpeed().x == -oldSpeedX);
    std::cout << "TestBrickCollisionEdge: PASS ✅\n";
}

// 额外加一个球和板的碰撞测试，保证这个功能也没问题
void TestPaddleCollision() {
    // 创建球，在板的正上方
    Ball ball({400.0f, 540.0f}, {0.0f, 5.0f}, 10.0f);
    Paddle paddle(340, 550, 120, 15);
    
    float oldSpeedY = ball.GetSpeed().y;
    
    // 执行板的碰撞
    ball.BouncePaddle(paddle.GetRect());
    
    // 碰撞之后球应该向上弹了
    assert(ball.GetSpeed().y < 0);
    std::cout << "TestPaddleCollision: PASS ✅\n";
}

int main() {
    std::cout << "=== 单元测试开始 ===\n";
    TestBallCreation();
    TestBrickCreation();
    TestBrickCollisionNormal();
    TestBrickCollisionEdge();
    TestPaddleCollision();
    std::cout << "=== 所有测试通过！===\n\n";
    // 暂停窗口，让你能看到测试结果
    pauseConsole();
    return 0;
}