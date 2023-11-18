cmake_minimum_required(VERSION 3.16)

project(yapng C CXX)

set(EXECUTABLE_NAME ${PROJECT_NAME})

include_directories(inc)

add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME}
PRIVATE
    src/main.cpp
)

add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_subdirectory(SDL_image EXCLUDE_FROM_ALL)

target_compile_features(${EXECUTABLE_NAME} PUBLIC)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC SDL3::SDL3 SDL3_image::SDL3_image)
