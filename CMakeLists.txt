cmake_minimum_required(VERSION 3.29)
project(Snake)

set(CMAKE_CXX_STANDARD 14)

# Specify the SDL2 folder
set(SDL2_PATH "${CMAKE_SOURCE_DIR}/SDL2-2.0.10/")
set(SDL2_INCLUDE_DIR "${SDL2_PATH}/include")
set(SDL2_LIB_DIR "${SDL2_PATH}/lib")

# Include SDL2 and project headers
include_directories(${SDL2_INCLUDE_DIR} include)

# Gather all source files
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp *.h)

# Add the executable
add_executable(Snake ${SOURCES})


target_link_libraries(Snake ${CMAKE_SOURCE_DIR}/libSDL2.a)

