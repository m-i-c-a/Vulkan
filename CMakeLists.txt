cmake_minimum_required(VERSION 3.0)
project(app)

find_package(glfw3 REQUIRED FATAL_ERROR)

set(CMAKE_BUILD_TYPE Debug)

add_executable( ${PROJECT_NAME} ${PROJECT_NAME}.cpp )
target_include_directories( ${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/include )
target_link_libraries( ${PROJECT_NAME} PRIVATE 
    $ENV{VULKAN_SDK}/lib/libvulkan.so
    glfw
)
