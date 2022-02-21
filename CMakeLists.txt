cmake_minimum_required(VERSION 3.1)
enable_language(CXX)
enable_testing()
set(CMAKE_CXX_STANDARD 17)
# デモにリンクするときだけコメントアウトを外す
#set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
project(EmotionalNES)

# DxLib 追加(テストフロントエンド用)
include_directories(${PROJECT_SOURCE_DIR}/lib/dxlib)
link_directories(${PROJECT_SOURCE_DIR}/lib/dxlib)

add_subdirectory(Programs)
add_subdirectory(Tests)