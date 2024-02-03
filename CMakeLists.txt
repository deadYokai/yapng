cmake_minimum_required(VERSION 3.16)

set(CXX_STANDARD 20)

# use clang by default
SET (CMAKE_C_COMPILER "clang")
SET (CMAKE_CXX_COMPILER "clang++")
SET (CMAKE_AR      "llvm-ar")
SET (CMAKE_LINKER  "llvm-ld")
SET (CMAKE_NM      "llvm-nm")
SET (CMAKE_OBJDUMP "llvm-objdump")
SET (CMAKE_RANLIB  "llvm-ranlib")

project(yapng C CXX)

set(EXECUTABLE_NAME ${PROJECT_NAME})

include_directories(include)

add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME} PRIVATE
        src/profile.cpp
        src/sprite.cpp
        src/slider.cpp
        src/saveManager.cpp
        src/main.cpp )

set(BUILD_SHARED_LIBS FALSE)
set(SDL3_FOUND TRUE)
set(SDL_SHARED FALSE)
set(SDL_STATIC TRUE)
add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_subdirectory(SDL_image EXCLUDE_FROM_ALL)

target_compile_features(${EXECUTABLE_NAME} PUBLIC)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC SDL3::SDL3-static SDL3_image::SDL3_image)


