# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_SOURCE_DIR = /home/heng/github/cpp-reactor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/heng/github/cpp-reactor/build

# Include any dependencies generated for this target.
include src/common/CMakeFiles/comm.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/common/CMakeFiles/comm.dir/compiler_depend.make

# Include the progress variables for this target.
include src/common/CMakeFiles/comm.dir/progress.make

# Include the compile flags for this target's objects.
include src/common/CMakeFiles/comm.dir/flags.make

src/common/CMakeFiles/comm.dir/Logger.cc.o: src/common/CMakeFiles/comm.dir/flags.make
src/common/CMakeFiles/comm.dir/Logger.cc.o: ../src/common/Logger.cc
src/common/CMakeFiles/comm.dir/Logger.cc.o: src/common/CMakeFiles/comm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/heng/github/cpp-reactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/common/CMakeFiles/comm.dir/Logger.cc.o"
	cd /home/heng/github/cpp-reactor/build/src/common && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/common/CMakeFiles/comm.dir/Logger.cc.o -MF CMakeFiles/comm.dir/Logger.cc.o.d -o CMakeFiles/comm.dir/Logger.cc.o -c /home/heng/github/cpp-reactor/src/common/Logger.cc

src/common/CMakeFiles/comm.dir/Logger.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/comm.dir/Logger.cc.i"
	cd /home/heng/github/cpp-reactor/build/src/common && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/heng/github/cpp-reactor/src/common/Logger.cc > CMakeFiles/comm.dir/Logger.cc.i

src/common/CMakeFiles/comm.dir/Logger.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/comm.dir/Logger.cc.s"
	cd /home/heng/github/cpp-reactor/build/src/common && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/heng/github/cpp-reactor/src/common/Logger.cc -o CMakeFiles/comm.dir/Logger.cc.s

src/common/CMakeFiles/comm.dir/TimeStamp.cc.o: src/common/CMakeFiles/comm.dir/flags.make
src/common/CMakeFiles/comm.dir/TimeStamp.cc.o: ../src/common/TimeStamp.cc
src/common/CMakeFiles/comm.dir/TimeStamp.cc.o: src/common/CMakeFiles/comm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/heng/github/cpp-reactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/common/CMakeFiles/comm.dir/TimeStamp.cc.o"
	cd /home/heng/github/cpp-reactor/build/src/common && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/common/CMakeFiles/comm.dir/TimeStamp.cc.o -MF CMakeFiles/comm.dir/TimeStamp.cc.o.d -o CMakeFiles/comm.dir/TimeStamp.cc.o -c /home/heng/github/cpp-reactor/src/common/TimeStamp.cc

src/common/CMakeFiles/comm.dir/TimeStamp.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/comm.dir/TimeStamp.cc.i"
	cd /home/heng/github/cpp-reactor/build/src/common && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/heng/github/cpp-reactor/src/common/TimeStamp.cc > CMakeFiles/comm.dir/TimeStamp.cc.i

src/common/CMakeFiles/comm.dir/TimeStamp.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/comm.dir/TimeStamp.cc.s"
	cd /home/heng/github/cpp-reactor/build/src/common && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/heng/github/cpp-reactor/src/common/TimeStamp.cc -o CMakeFiles/comm.dir/TimeStamp.cc.s

# Object files for target comm
comm_OBJECTS = \
"CMakeFiles/comm.dir/Logger.cc.o" \
"CMakeFiles/comm.dir/TimeStamp.cc.o"

# External object files for target comm
comm_EXTERNAL_OBJECTS =

../lib/libcomm.so: src/common/CMakeFiles/comm.dir/Logger.cc.o
../lib/libcomm.so: src/common/CMakeFiles/comm.dir/TimeStamp.cc.o
../lib/libcomm.so: src/common/CMakeFiles/comm.dir/build.make
../lib/libcomm.so: src/common/CMakeFiles/comm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/heng/github/cpp-reactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX shared library ../../../lib/libcomm.so"
	cd /home/heng/github/cpp-reactor/build/src/common && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/comm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/common/CMakeFiles/comm.dir/build: ../lib/libcomm.so
.PHONY : src/common/CMakeFiles/comm.dir/build

src/common/CMakeFiles/comm.dir/clean:
	cd /home/heng/github/cpp-reactor/build/src/common && $(CMAKE_COMMAND) -P CMakeFiles/comm.dir/cmake_clean.cmake
.PHONY : src/common/CMakeFiles/comm.dir/clean

src/common/CMakeFiles/comm.dir/depend:
	cd /home/heng/github/cpp-reactor/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/heng/github/cpp-reactor /home/heng/github/cpp-reactor/src/common /home/heng/github/cpp-reactor/build /home/heng/github/cpp-reactor/build/src/common /home/heng/github/cpp-reactor/build/src/common/CMakeFiles/comm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/common/CMakeFiles/comm.dir/depend

