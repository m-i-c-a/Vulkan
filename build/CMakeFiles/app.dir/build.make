# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mica/Desktop/Vulkan/Vulkan

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mica/Desktop/Vulkan/Vulkan/build

# Include any dependencies generated for this target.
include CMakeFiles/app.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/app.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/app.dir/flags.make

CMakeFiles/app.dir/main.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/app.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/app.dir/main.cpp.o -c /home/mica/Desktop/Vulkan/Vulkan/main.cpp

CMakeFiles/app.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/app.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan/main.cpp > CMakeFiles/app.dir/main.cpp.i

CMakeFiles/app.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/app.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan/main.cpp -o CMakeFiles/app.dir/main.cpp.s

CMakeFiles/app.dir/Renderer/SortBin.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/Renderer/SortBin.cpp.o: ../Renderer/SortBin.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/app.dir/Renderer/SortBin.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/app.dir/Renderer/SortBin.cpp.o -c /home/mica/Desktop/Vulkan/Vulkan/Renderer/SortBin.cpp

CMakeFiles/app.dir/Renderer/SortBin.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/app.dir/Renderer/SortBin.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan/Renderer/SortBin.cpp > CMakeFiles/app.dir/Renderer/SortBin.cpp.i

CMakeFiles/app.dir/Renderer/SortBin.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/app.dir/Renderer/SortBin.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan/Renderer/SortBin.cpp -o CMakeFiles/app.dir/Renderer/SortBin.cpp.s

CMakeFiles/app.dir/Renderer/PipelineBin.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/Renderer/PipelineBin.cpp.o: ../Renderer/PipelineBin.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/app.dir/Renderer/PipelineBin.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/app.dir/Renderer/PipelineBin.cpp.o -c /home/mica/Desktop/Vulkan/Vulkan/Renderer/PipelineBin.cpp

CMakeFiles/app.dir/Renderer/PipelineBin.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/app.dir/Renderer/PipelineBin.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan/Renderer/PipelineBin.cpp > CMakeFiles/app.dir/Renderer/PipelineBin.cpp.i

CMakeFiles/app.dir/Renderer/PipelineBin.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/app.dir/Renderer/PipelineBin.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan/Renderer/PipelineBin.cpp -o CMakeFiles/app.dir/Renderer/PipelineBin.cpp.s

CMakeFiles/app.dir/Renderer/Renderable.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/Renderer/Renderable.cpp.o: ../Renderer/Renderable.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/app.dir/Renderer/Renderable.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/app.dir/Renderer/Renderable.cpp.o -c /home/mica/Desktop/Vulkan/Vulkan/Renderer/Renderable.cpp

CMakeFiles/app.dir/Renderer/Renderable.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/app.dir/Renderer/Renderable.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan/Renderer/Renderable.cpp > CMakeFiles/app.dir/Renderer/Renderable.cpp.i

CMakeFiles/app.dir/Renderer/Renderable.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/app.dir/Renderer/Renderable.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan/Renderer/Renderable.cpp -o CMakeFiles/app.dir/Renderer/Renderable.cpp.s

CMakeFiles/app.dir/Renderer/PipelineManager.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/Renderer/PipelineManager.cpp.o: ../Renderer/PipelineManager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/app.dir/Renderer/PipelineManager.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/app.dir/Renderer/PipelineManager.cpp.o -c /home/mica/Desktop/Vulkan/Vulkan/Renderer/PipelineManager.cpp

CMakeFiles/app.dir/Renderer/PipelineManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/app.dir/Renderer/PipelineManager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan/Renderer/PipelineManager.cpp > CMakeFiles/app.dir/Renderer/PipelineManager.cpp.i

CMakeFiles/app.dir/Renderer/PipelineManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/app.dir/Renderer/PipelineManager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan/Renderer/PipelineManager.cpp -o CMakeFiles/app.dir/Renderer/PipelineManager.cpp.s

CMakeFiles/app.dir/App.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/App.cpp.o: ../App.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/app.dir/App.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/app.dir/App.cpp.o -c /home/mica/Desktop/Vulkan/Vulkan/App.cpp

CMakeFiles/app.dir/App.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/app.dir/App.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan/App.cpp > CMakeFiles/app.dir/App.cpp.i

CMakeFiles/app.dir/App.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/app.dir/App.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan/App.cpp -o CMakeFiles/app.dir/App.cpp.s

CMakeFiles/app.dir/VkStartup.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/VkStartup.cpp.o: ../VkStartup.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/app.dir/VkStartup.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/app.dir/VkStartup.cpp.o -c /home/mica/Desktop/Vulkan/Vulkan/VkStartup.cpp

CMakeFiles/app.dir/VkStartup.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/app.dir/VkStartup.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan/VkStartup.cpp > CMakeFiles/app.dir/VkStartup.cpp.i

CMakeFiles/app.dir/VkStartup.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/app.dir/VkStartup.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan/VkStartup.cpp -o CMakeFiles/app.dir/VkStartup.cpp.s

CMakeFiles/app.dir/VkRuntime.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/VkRuntime.cpp.o: ../VkRuntime.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/app.dir/VkRuntime.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/app.dir/VkRuntime.cpp.o -c /home/mica/Desktop/Vulkan/Vulkan/VkRuntime.cpp

CMakeFiles/app.dir/VkRuntime.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/app.dir/VkRuntime.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan/VkRuntime.cpp > CMakeFiles/app.dir/VkRuntime.cpp.i

CMakeFiles/app.dir/VkRuntime.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/app.dir/VkRuntime.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan/VkRuntime.cpp -o CMakeFiles/app.dir/VkRuntime.cpp.s

CMakeFiles/app.dir/VkFrame.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/VkFrame.cpp.o: ../VkFrame.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/app.dir/VkFrame.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/app.dir/VkFrame.cpp.o -c /home/mica/Desktop/Vulkan/Vulkan/VkFrame.cpp

CMakeFiles/app.dir/VkFrame.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/app.dir/VkFrame.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan/VkFrame.cpp > CMakeFiles/app.dir/VkFrame.cpp.i

CMakeFiles/app.dir/VkFrame.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/app.dir/VkFrame.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan/VkFrame.cpp -o CMakeFiles/app.dir/VkFrame.cpp.s

CMakeFiles/app.dir/Loader.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/Loader.cpp.o: ../Loader.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/app.dir/Loader.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/app.dir/Loader.cpp.o -c /home/mica/Desktop/Vulkan/Vulkan/Loader.cpp

CMakeFiles/app.dir/Loader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/app.dir/Loader.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan/Loader.cpp > CMakeFiles/app.dir/Loader.cpp.i

CMakeFiles/app.dir/Loader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/app.dir/Loader.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan/Loader.cpp -o CMakeFiles/app.dir/Loader.cpp.s

CMakeFiles/app.dir/Buffer.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/Buffer.cpp.o: ../Buffer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/app.dir/Buffer.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/app.dir/Buffer.cpp.o -c /home/mica/Desktop/Vulkan/Vulkan/Buffer.cpp

CMakeFiles/app.dir/Buffer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/app.dir/Buffer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan/Buffer.cpp > CMakeFiles/app.dir/Buffer.cpp.i

CMakeFiles/app.dir/Buffer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/app.dir/Buffer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan/Buffer.cpp -o CMakeFiles/app.dir/Buffer.cpp.s

# Object files for target app
app_OBJECTS = \
"CMakeFiles/app.dir/main.cpp.o" \
"CMakeFiles/app.dir/Renderer/SortBin.cpp.o" \
"CMakeFiles/app.dir/Renderer/PipelineBin.cpp.o" \
"CMakeFiles/app.dir/Renderer/Renderable.cpp.o" \
"CMakeFiles/app.dir/Renderer/PipelineManager.cpp.o" \
"CMakeFiles/app.dir/App.cpp.o" \
"CMakeFiles/app.dir/VkStartup.cpp.o" \
"CMakeFiles/app.dir/VkRuntime.cpp.o" \
"CMakeFiles/app.dir/VkFrame.cpp.o" \
"CMakeFiles/app.dir/Loader.cpp.o" \
"CMakeFiles/app.dir/Buffer.cpp.o"

# External object files for target app
app_EXTERNAL_OBJECTS =

app: CMakeFiles/app.dir/main.cpp.o
app: CMakeFiles/app.dir/Renderer/SortBin.cpp.o
app: CMakeFiles/app.dir/Renderer/PipelineBin.cpp.o
app: CMakeFiles/app.dir/Renderer/Renderable.cpp.o
app: CMakeFiles/app.dir/Renderer/PipelineManager.cpp.o
app: CMakeFiles/app.dir/App.cpp.o
app: CMakeFiles/app.dir/VkStartup.cpp.o
app: CMakeFiles/app.dir/VkRuntime.cpp.o
app: CMakeFiles/app.dir/VkFrame.cpp.o
app: CMakeFiles/app.dir/Loader.cpp.o
app: CMakeFiles/app.dir/Buffer.cpp.o
app: CMakeFiles/app.dir/build.make
app: /home/mica/Downloads/1.3.204.1/x86_64/lib/libvulkan.so
app: /usr/lib/x86_64-linux-gnu/libglfw.so.3.3
app: CMakeFiles/app.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mica/Desktop/Vulkan/Vulkan/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Linking CXX executable app"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/app.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/app.dir/build: app

.PHONY : CMakeFiles/app.dir/build

CMakeFiles/app.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/app.dir/cmake_clean.cmake
.PHONY : CMakeFiles/app.dir/clean

CMakeFiles/app.dir/depend:
	cd /home/mica/Desktop/Vulkan/Vulkan/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mica/Desktop/Vulkan/Vulkan /home/mica/Desktop/Vulkan/Vulkan /home/mica/Desktop/Vulkan/Vulkan/build /home/mica/Desktop/Vulkan/Vulkan/build /home/mica/Desktop/Vulkan/Vulkan/build/CMakeFiles/app.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/app.dir/depend

