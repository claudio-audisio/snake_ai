cmake_minimum_required(VERSION 4.1)
project(snake_ai)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto=auto -DNDEBUG -fopenmp")

add_library(snakeLib INTERFACE)
target_include_directories(snakeLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(snakeLib INTERFACE ../tiny-dnn)

add_executable(snake_ai snake_ai.cpp)

target_link_libraries(snake_ai PRIVATE -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -fopenmp)
target_link_libraries(snake_ai PRIVATE snakeLib)
