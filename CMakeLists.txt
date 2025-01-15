cmake_minimum_required(VERSION 3.20)
project(Chip8)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory(src)

target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})




message(STATUS "Current source directory: ${CMAKE_SOURCE_DIR}")
install(TARGETS Chip8 DESTINATION ${CMAKE_SOURCE_DIR})
