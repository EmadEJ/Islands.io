cmake_minimum_required(VERSION 3.21)
project(test_graphics C)

set(CMAKE_C_STANDARD 11)

add_executable(test_graphics main.c Map.c Map.h Functions.c Functions.h Perimeters.h FileHandling.c FileHandling.h Texture.c Texture.h Scoreboard.c Scoreboard.h Menu.c Menu.h )

include_directories("/usr/include/SDL2")
target_link_libraries(test_graphics m SDL2 SDL2_gfx SDL2_ttf)
