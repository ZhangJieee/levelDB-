# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_COMMAND = /data00/cmake-3.13.0/bin/cmake

# The command to remove a file.
RM = /data00/cmake-3.13.0/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zhangjie.tzhangj/leveldb

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zhangjie.tzhangj/leveldb/build

# Include any dependencies generated for this target.
include CMakeFiles/no_destructor_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/no_destructor_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/no_destructor_test.dir/flags.make

CMakeFiles/no_destructor_test.dir/util/testutil.cc.o: CMakeFiles/no_destructor_test.dir/flags.make
CMakeFiles/no_destructor_test.dir/util/testutil.cc.o: ../util/testutil.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zhangjie.tzhangj/leveldb/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/no_destructor_test.dir/util/testutil.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/no_destructor_test.dir/util/testutil.cc.o -c /home/zhangjie.tzhangj/leveldb/util/testutil.cc

CMakeFiles/no_destructor_test.dir/util/testutil.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/no_destructor_test.dir/util/testutil.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zhangjie.tzhangj/leveldb/util/testutil.cc > CMakeFiles/no_destructor_test.dir/util/testutil.cc.i

CMakeFiles/no_destructor_test.dir/util/testutil.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/no_destructor_test.dir/util/testutil.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zhangjie.tzhangj/leveldb/util/testutil.cc -o CMakeFiles/no_destructor_test.dir/util/testutil.cc.s

CMakeFiles/no_destructor_test.dir/util/no_destructor_test.cc.o: CMakeFiles/no_destructor_test.dir/flags.make
CMakeFiles/no_destructor_test.dir/util/no_destructor_test.cc.o: ../util/no_destructor_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zhangjie.tzhangj/leveldb/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/no_destructor_test.dir/util/no_destructor_test.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/no_destructor_test.dir/util/no_destructor_test.cc.o -c /home/zhangjie.tzhangj/leveldb/util/no_destructor_test.cc

CMakeFiles/no_destructor_test.dir/util/no_destructor_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/no_destructor_test.dir/util/no_destructor_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zhangjie.tzhangj/leveldb/util/no_destructor_test.cc > CMakeFiles/no_destructor_test.dir/util/no_destructor_test.cc.i

CMakeFiles/no_destructor_test.dir/util/no_destructor_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/no_destructor_test.dir/util/no_destructor_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zhangjie.tzhangj/leveldb/util/no_destructor_test.cc -o CMakeFiles/no_destructor_test.dir/util/no_destructor_test.cc.s

# Object files for target no_destructor_test
no_destructor_test_OBJECTS = \
"CMakeFiles/no_destructor_test.dir/util/testutil.cc.o" \
"CMakeFiles/no_destructor_test.dir/util/no_destructor_test.cc.o"

# External object files for target no_destructor_test
no_destructor_test_EXTERNAL_OBJECTS =

no_destructor_test: CMakeFiles/no_destructor_test.dir/util/testutil.cc.o
no_destructor_test: CMakeFiles/no_destructor_test.dir/util/no_destructor_test.cc.o
no_destructor_test: CMakeFiles/no_destructor_test.dir/build.make
no_destructor_test: libleveldb.a
no_destructor_test: lib/libgmock.a
no_destructor_test: lib/libgtest.a
no_destructor_test: third_party/benchmark/src/libbenchmark.a
no_destructor_test: /usr/lib/x86_64-linux-gnu/librt.so
no_destructor_test: CMakeFiles/no_destructor_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zhangjie.tzhangj/leveldb/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable no_destructor_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/no_destructor_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/no_destructor_test.dir/build: no_destructor_test

.PHONY : CMakeFiles/no_destructor_test.dir/build

CMakeFiles/no_destructor_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/no_destructor_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/no_destructor_test.dir/clean

CMakeFiles/no_destructor_test.dir/depend:
	cd /home/zhangjie.tzhangj/leveldb/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zhangjie.tzhangj/leveldb /home/zhangjie.tzhangj/leveldb /home/zhangjie.tzhangj/leveldb/build /home/zhangjie.tzhangj/leveldb/build /home/zhangjie.tzhangj/leveldb/build/CMakeFiles/no_destructor_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/no_destructor_test.dir/depend

