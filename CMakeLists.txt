cmake_minimum_required(VERSION 3.16)

project(yapng C CXX)

set(EXECUTABLE_NAME ${PROJECT_NAME})

option(WINDOW_FLAG_OPENGL "Use OpenGL instead Vulkan" NO)

include_directories(inc)

add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME} PRIVATE src/main.cpp)

add_subdirectory(SDL EXCLUDE_FROM_ALL)
set(SDL3_FOUND TRUE)
add_subdirectory(SDL_image EXCLUDE_FROM_ALL)

if(WINDOW_FLAG_OPENGL)
    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC WINDOW_FLAG_OPENGL)
endif()

target_compile_features(${EXECUTABLE_NAME} PUBLIC)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC SDL3::SDL3 SDL3_image::SDL3_image)


