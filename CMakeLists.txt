cmake_minimum_required(VERSION 3.12)

project(cbump VERSION 0.1)

add_library(cbump src/cbump.h src/cbump.c)
target_include_directories(cbump PUBLIC src)

add_executable(pong examples/pong.c)
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(pong cbump SDL2 m)

add_executable(test examples/test.c)
target_link_libraries(test cbump)
