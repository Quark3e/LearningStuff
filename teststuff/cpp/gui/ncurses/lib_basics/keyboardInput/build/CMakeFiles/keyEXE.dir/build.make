# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/Chromebook-projects/teststuff/cpp/gui/ncurses/lib_basics/keyboardInput

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/Chromebook-projects/teststuff/cpp/gui/ncurses/lib_basics/keyboardInput/build

# Include any dependencies generated for this target.
include CMakeFiles/keyEXE.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/keyEXE.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/keyEXE.dir/flags.make

CMakeFiles/keyEXE.dir/main.cpp.o: CMakeFiles/keyEXE.dir/flags.make
CMakeFiles/keyEXE.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/Chromebook-projects/teststuff/cpp/gui/ncurses/lib_basics/keyboardInput/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/keyEXE.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/keyEXE.dir/main.cpp.o -c /home/pi/Chromebook-projects/teststuff/cpp/gui/ncurses/lib_basics/keyboardInput/main.cpp

CMakeFiles/keyEXE.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/keyEXE.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/pi/Chromebook-projects/teststuff/cpp/gui/ncurses/lib_basics/keyboardInput/main.cpp > CMakeFiles/keyEXE.dir/main.cpp.i

CMakeFiles/keyEXE.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/keyEXE.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/pi/Chromebook-projects/teststuff/cpp/gui/ncurses/lib_basics/keyboardInput/main.cpp -o CMakeFiles/keyEXE.dir/main.cpp.s

# Object files for target keyEXE
keyEXE_OBJECTS = \
"CMakeFiles/keyEXE.dir/main.cpp.o"

# External object files for target keyEXE
keyEXE_EXTERNAL_OBJECTS =

keyEXE: CMakeFiles/keyEXE.dir/main.cpp.o
keyEXE: CMakeFiles/keyEXE.dir/build.make
keyEXE: CMakeFiles/keyEXE.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/Chromebook-projects/teststuff/cpp/gui/ncurses/lib_basics/keyboardInput/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable keyEXE"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/keyEXE.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/keyEXE.dir/build: keyEXE

.PHONY : CMakeFiles/keyEXE.dir/build

CMakeFiles/keyEXE.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/keyEXE.dir/cmake_clean.cmake
.PHONY : CMakeFiles/keyEXE.dir/clean

CMakeFiles/keyEXE.dir/depend:
	cd /home/pi/Chromebook-projects/teststuff/cpp/gui/ncurses/lib_basics/keyboardInput/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/Chromebook-projects/teststuff/cpp/gui/ncurses/lib_basics/keyboardInput /home/pi/Chromebook-projects/teststuff/cpp/gui/ncurses/lib_basics/keyboardInput /home/pi/Chromebook-projects/teststuff/cpp/gui/ncurses/lib_basics/keyboardInput/build /home/pi/Chromebook-projects/teststuff/cpp/gui/ncurses/lib_basics/keyboardInput/build /home/pi/Chromebook-projects/teststuff/cpp/gui/ncurses/lib_basics/keyboardInput/build/CMakeFiles/keyEXE.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/keyEXE.dir/depend

