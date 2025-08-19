cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
    sqlite-demo
    VERSION 1.0
    LANGUAGES CXX)

add_subdirectory(lib)

add_executable(sqlite-demo src/main.cpp)

target_link_libraries(sqlite-demo PRIVATE SQLiteWrap)
