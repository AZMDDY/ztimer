cmake_minimum_required(VERSION 3.10.0)

project(example_ztimer)

include_directories(./include)

link_directories(./lib)

add_executable(example_ztimer main.cpp)

target_link_libraries(example_ztimer ztimer pthread)