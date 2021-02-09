cmake_minimum_required(VERSION 3.13)
project(dcfsm C)

set(CMAKE_C_STANDARD 11)

add_library(lib STATIC fsm.h fsm.c)
target_compile_options(lib PRIVATE)

add_executable(client client.c)
target_link_libraries(client lib)
target_compile_options(client PRIVATE -Wpedantic -Wall -Wextra)

add_executable(server server.c)
target_link_libraries(server lib)
target_compile_options(server PRIVATE -Wpedantic -Wall -Wextra)

