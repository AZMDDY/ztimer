cmake_minimum_required(VERSION 3.10.0)

project(ztimer)

include_directories(./)

add_executable(ztimer main.cpp ztimer/TimerManager.cpp ztimer/Timer.cpp)

target_link_libraries(ztimer pthread)