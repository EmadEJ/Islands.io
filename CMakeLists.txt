cmake_minimum_required(VERSION 3.21)
project(Islands.io C)

set(CMAKE_C_STANDARD 11)

add_executable(Islands.io main.c lib/Map.c lib/Map.h lib/Functions.c lib/Functions.h lib/Perimeters.h lib/FileHandling.c lib/FileHandling.h lib/Texture.c lib/Texture.h lib/Scoreboard.c lib/Scoreboard.h lib/Menu.c lib/Menu.h)

include_directories("/usr/include/SDL2")
target_link_libraries(Islands.io m SDL2 SDL2_gfx SDL2_ttf)
