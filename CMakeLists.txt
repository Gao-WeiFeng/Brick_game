cmake_minimum_required(VERSION 3.10)
project(BrickGame)
# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 精准匹配你的raylib路径和版本，我完全保留了你原来的设置！
set(RAYLIB_DIR "E:/software/raylib")
include_directories(${RAYLIB_DIR}/include)
link_directories(${RAYLIB_DIR}/lib)
# 你的游戏源文件，加了新的PowerUp.cpp
set(SOURCES
    src/main.cpp
    src/Ball.cpp
    src/Paddle.cpp
    src/Brick.cpp
    src/Game.cpp
    src/PowerUp.cpp
)
add_executable(${PROJECT_NAME} ${SOURCES})
# 链接 Raylib 和 Windows 必须依赖
target_link_libraries(${PROJECT_NAME} raylib winmm gdi32 opengl32)
# 单元测试目标
add_executable(test
    src/test.cpp
    src/Ball.cpp
    src/Paddle.cpp
    src/Brick.cpp
)
# 链接 raylib 库
target_include_directories(test PRIVATE ${RAYLIB_DIR}/include)
target_link_libraries(test PRIVATE raylib winmm gdi32 opengl32)