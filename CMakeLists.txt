cmake_minimum_required(VERSION 3.0)
project(app)

find_package(glfw3 REQUIRED FATAL_ERROR)

set(CMAKE_BUILD_TYPE Debug)

message(STATUS "Using " $ENV{VULKAN_SDK})

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
add_subdirectory( ${CMAKE_HOME_DIRECTORY}/extern/tinygltf )

add_executable( ${PROJECT_NAME} main.cpp
    Renderer/RenderManager.hpp
    Renderer/SortBin.cpp         Renderer/SortBin.hpp
    Renderer/PipelineBin.cpp     Renderer/PipelineBin.hpp
    Renderer/Renderable.cpp      Renderer/Renderable.hpp
    Renderer/PipelineCreator.cpp Renderer/PipelineCreator.hpp

    App.cpp App.hpp
    VkStartup.cpp VkStartup.hpp
    VkRuntime.cpp VkRuntime.hpp
    VkFrame.cpp VkFrame.hpp
    Loader.cpp Loader.hpp
    Buffer.cpp Buffer.hpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories( ${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/include ${CMAKE_HOME_DIRECTORY}/extern )
target_link_libraries( ${PROJECT_NAME} PRIVATE 
    $ENV{VULKAN_SDK}/lib/libvulkan.so
    glfw
)
