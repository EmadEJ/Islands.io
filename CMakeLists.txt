cmake_minimum_required(VERSION 3.21)
project(test_graphics C)

set(CMAKE_C_STANDARD 11)

add_executable(test_graphics main.c Map.h Functions.h Perimeters.h)

include_directories("/usr/include/SDL2")
target_link_libraries(test_graphics m SDL2 SDL2_gfx SDL2_ttf)
