cmake_minimum_required(VERSION 3.0)
project(app)

find_package(glfw3 REQUIRED FATAL_ERROR)

set(CMAKE_BUILD_TYPE Debug)


set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
add_subdirectory( ${CMAKE_HOME_DIRECTORY}/extern/tinygltf )

add_executable( ${PROJECT_NAME} ${PROJECT_NAME}.cpp
    Loader.cpp Loader.hpp
    Buffer.cpp Buffer.hpp
)

target_include_directories( ${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/include ${CMAKE_HOME_DIRECTORY}/extern )
target_link_libraries( ${PROJECT_NAME} PRIVATE 
    $ENV{VULKAN_SDK}/lib/libvulkan.so
    glfw
)
