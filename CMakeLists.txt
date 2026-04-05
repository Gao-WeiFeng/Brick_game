cmake_minimum_required(VERSION 3.10)
project(BrickGame)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 你的raylib路径（完全匹配你之前的）
set(RAYLIB_DIR "E:/software/raylib")
include_directories(${RAYLIB_DIR}/include)
link_directories(${RAYLIB_DIR}/lib)

# 所有源文件（加了新的Game.cpp）
set(SOURCES
    src/main.cpp
    src/Ball.cpp
    src/Paddle.cpp
    src/Brick.cpp
    src/Game.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# 链接依赖库
target_link_libraries(${PROJECT_NAME} raylib winmm gdi32 opengl32)

# 单元测试目标（之前的测试）
add_executable(test
    src/test.cpp
    src/Ball.cpp
    src/Paddle.cpp
    src/Brick.cpp
)
target_include_directories(test PRIVATE ${RAYLIB_DIR}/include)
target_link_libraries(test PRIVATE raylib winmm gdi32 opengl32)