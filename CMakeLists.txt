cmake_minimum_required(VERSION 3.10)
project(breakout_week2)
 # 固定 C++17 标准
 set(CMAKE_CXX_STANDARD 17)
 set(CMAKE_CXX_STANDARD_REQUIRED ON)
 # --------------------------
 # 精准匹配你的 Raylib 路径和版本
 # --------------------------
 set(RAYLIB_DIR "E:/software/raylib")
 include_directories(${RAYLIB_DIR}/include)
 link_directories(${RAYLIB_DIR}/lib)
 # --------------------------
 # 你的游戏源文件（把所有 .cpp 都列全）
 # --------------------------
 set(SOURCES
     src/main.cpp
     src/Ball.cpp
     src/Paddle.cpp
     src/Brick.cpp
 )
 add_executable(${PROJECT_NAME} ${SOURCES})
 # --------------------------
 # 链接 Raylib 和 Windows 必需依赖
 # --------------------------
 target_link_libraries(${PROJECT_NAME} raylib winmm gdi32 opengl32)