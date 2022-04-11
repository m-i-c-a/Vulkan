cmake_minimum_required(VERSION 3.0)
project(app)

find_package(glfw3 REQUIRED FATAL_ERROR)
find_package(Vulkan REQUIRED FATAL_ERROR)

message( STATUS ${Vulkan_INCLUDE_DIRS} )

set(CMAKE_BUILD_TYPE Debug)

add_executable( ${PROJECT_NAME} ${PROJECT_NAME}.cpp )
target_include_directories( ${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} PRIVATE 
    ${Vulkan_LIBRARY} 
    glfw
)
