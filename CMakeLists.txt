cmake_minimum_required(VERSION 3.4)

project(whackamole)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")

add_executable(twitch ukol.c sdl.c dynamic_array.c)
target_link_libraries(twitch SDL2 SDL2_image SDL2_ttf m)