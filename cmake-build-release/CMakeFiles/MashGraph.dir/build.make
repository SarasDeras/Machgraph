# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.16

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.1\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.1\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\Arseniy\CLionProjects\MashGraph

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\Arseniy\CLionProjects\MashGraph\cmake-build-release

# Include any dependencies generated for this target.
include CMakeFiles/MashGraph.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MashGraph.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MashGraph.dir/flags.make

CMakeFiles/MashGraph.dir/main.cpp.obj: CMakeFiles/MashGraph.dir/flags.make
CMakeFiles/MashGraph.dir/main.cpp.obj: CMakeFiles/MashGraph.dir/includes_CXX.rsp
CMakeFiles/MashGraph.dir/main.cpp.obj: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Arseniy\CLionProjects\MashGraph\cmake-build-release\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/MashGraph.dir/main.cpp.obj"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\G__~1.EXE  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\MashGraph.dir\main.cpp.obj -c C:\Users\Arseniy\CLionProjects\MashGraph\main.cpp

CMakeFiles/MashGraph.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MashGraph.dir/main.cpp.i"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\Arseniy\CLionProjects\MashGraph\main.cpp > CMakeFiles\MashGraph.dir\main.cpp.i

CMakeFiles/MashGraph.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MashGraph.dir/main.cpp.s"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\Arseniy\CLionProjects\MashGraph\main.cpp -o CMakeFiles\MashGraph.dir\main.cpp.s

# Object files for target MashGraph
MashGraph_OBJECTS = \
"CMakeFiles/MashGraph.dir/main.cpp.obj"

# External object files for target MashGraph
MashGraph_EXTERNAL_OBJECTS =

MashGraph.exe: CMakeFiles/MashGraph.dir/main.cpp.obj
MashGraph.exe: CMakeFiles/MashGraph.dir/build.make
MashGraph.exe: CMakeFiles/MashGraph.dir/linklibs.rsp
MashGraph.exe: CMakeFiles/MashGraph.dir/objects1.rsp
MashGraph.exe: CMakeFiles/MashGraph.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\Arseniy\CLionProjects\MashGraph\cmake-build-release\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable MashGraph.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\MashGraph.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MashGraph.dir/build: MashGraph.exe

.PHONY : CMakeFiles/MashGraph.dir/build

CMakeFiles/MashGraph.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\MashGraph.dir\cmake_clean.cmake
.PHONY : CMakeFiles/MashGraph.dir/clean

CMakeFiles/MashGraph.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\Arseniy\CLionProjects\MashGraph C:\Users\Arseniy\CLionProjects\MashGraph C:\Users\Arseniy\CLionProjects\MashGraph\cmake-build-release C:\Users\Arseniy\CLionProjects\MashGraph\cmake-build-release C:\Users\Arseniy\CLionProjects\MashGraph\cmake-build-release\CMakeFiles\MashGraph.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MashGraph.dir/depend

