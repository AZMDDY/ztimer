cmake_minimum_required(VERSION 3.10.0)

project(ztimer)

# 编译警告选项
add_compile_options(-Wall)
add_compile_options(-Wconversion)
add_compile_options(-Wuninitialized)
add_compile_options(-Werror)

set(CMAKE_CXX_STANDARD 14)

SET(CMAKE_BUILD_TYPE "Release")

include_directories(./ztimer)

option(TARGET "taget" none)

if(${TARGET} STREQUAL "examples")
    add_executable(example_ztimer examples/main.cpp ztimer/TimerManager.cpp ztimer/Timer.cpp)
    target_link_libraries(example_ztimer pthread)
else()
add_library(ztimer SHARED ztimer/TimerManager.cpp ztimer/Timer.cpp)

target_link_libraries(ztimer pthread)

install(TARGETS ztimer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}./
)

install(FILES ztimer/Timer.h DESTINATION include/ztimer/)
endif()
