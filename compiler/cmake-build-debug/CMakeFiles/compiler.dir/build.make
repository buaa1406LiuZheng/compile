# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/liuzheng/Public/ClionProjects/compiler

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/liuzheng/Public/ClionProjects/compiler/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/compiler.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/compiler.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/compiler.dir/flags.make

CMakeFiles/compiler.dir/main.c.o: CMakeFiles/compiler.dir/flags.make
CMakeFiles/compiler.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/liuzheng/Public/ClionProjects/compiler/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/compiler.dir/main.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compiler.dir/main.c.o   -c /Users/liuzheng/Public/ClionProjects/compiler/main.c

CMakeFiles/compiler.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compiler.dir/main.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/liuzheng/Public/ClionProjects/compiler/main.c > CMakeFiles/compiler.dir/main.c.i

CMakeFiles/compiler.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compiler.dir/main.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/liuzheng/Public/ClionProjects/compiler/main.c -o CMakeFiles/compiler.dir/main.c.s

CMakeFiles/compiler.dir/main.c.o.requires:

.PHONY : CMakeFiles/compiler.dir/main.c.o.requires

CMakeFiles/compiler.dir/main.c.o.provides: CMakeFiles/compiler.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/compiler.dir/build.make CMakeFiles/compiler.dir/main.c.o.provides.build
.PHONY : CMakeFiles/compiler.dir/main.c.o.provides

CMakeFiles/compiler.dir/main.c.o.provides.build: CMakeFiles/compiler.dir/main.c.o


CMakeFiles/compiler.dir/lexical.c.o: CMakeFiles/compiler.dir/flags.make
CMakeFiles/compiler.dir/lexical.c.o: ../lexical.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/liuzheng/Public/ClionProjects/compiler/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/compiler.dir/lexical.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compiler.dir/lexical.c.o   -c /Users/liuzheng/Public/ClionProjects/compiler/lexical.c

CMakeFiles/compiler.dir/lexical.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compiler.dir/lexical.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/liuzheng/Public/ClionProjects/compiler/lexical.c > CMakeFiles/compiler.dir/lexical.c.i

CMakeFiles/compiler.dir/lexical.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compiler.dir/lexical.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/liuzheng/Public/ClionProjects/compiler/lexical.c -o CMakeFiles/compiler.dir/lexical.c.s

CMakeFiles/compiler.dir/lexical.c.o.requires:

.PHONY : CMakeFiles/compiler.dir/lexical.c.o.requires

CMakeFiles/compiler.dir/lexical.c.o.provides: CMakeFiles/compiler.dir/lexical.c.o.requires
	$(MAKE) -f CMakeFiles/compiler.dir/build.make CMakeFiles/compiler.dir/lexical.c.o.provides.build
.PHONY : CMakeFiles/compiler.dir/lexical.c.o.provides

CMakeFiles/compiler.dir/lexical.c.o.provides.build: CMakeFiles/compiler.dir/lexical.c.o


CMakeFiles/compiler.dir/error.c.o: CMakeFiles/compiler.dir/flags.make
CMakeFiles/compiler.dir/error.c.o: ../error.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/liuzheng/Public/ClionProjects/compiler/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/compiler.dir/error.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compiler.dir/error.c.o   -c /Users/liuzheng/Public/ClionProjects/compiler/error.c

CMakeFiles/compiler.dir/error.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compiler.dir/error.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/liuzheng/Public/ClionProjects/compiler/error.c > CMakeFiles/compiler.dir/error.c.i

CMakeFiles/compiler.dir/error.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compiler.dir/error.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/liuzheng/Public/ClionProjects/compiler/error.c -o CMakeFiles/compiler.dir/error.c.s

CMakeFiles/compiler.dir/error.c.o.requires:

.PHONY : CMakeFiles/compiler.dir/error.c.o.requires

CMakeFiles/compiler.dir/error.c.o.provides: CMakeFiles/compiler.dir/error.c.o.requires
	$(MAKE) -f CMakeFiles/compiler.dir/build.make CMakeFiles/compiler.dir/error.c.o.provides.build
.PHONY : CMakeFiles/compiler.dir/error.c.o.provides

CMakeFiles/compiler.dir/error.c.o.provides.build: CMakeFiles/compiler.dir/error.c.o


CMakeFiles/compiler.dir/syntax.c.o: CMakeFiles/compiler.dir/flags.make
CMakeFiles/compiler.dir/syntax.c.o: ../syntax.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/liuzheng/Public/ClionProjects/compiler/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/compiler.dir/syntax.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compiler.dir/syntax.c.o   -c /Users/liuzheng/Public/ClionProjects/compiler/syntax.c

CMakeFiles/compiler.dir/syntax.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compiler.dir/syntax.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/liuzheng/Public/ClionProjects/compiler/syntax.c > CMakeFiles/compiler.dir/syntax.c.i

CMakeFiles/compiler.dir/syntax.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compiler.dir/syntax.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/liuzheng/Public/ClionProjects/compiler/syntax.c -o CMakeFiles/compiler.dir/syntax.c.s

CMakeFiles/compiler.dir/syntax.c.o.requires:

.PHONY : CMakeFiles/compiler.dir/syntax.c.o.requires

CMakeFiles/compiler.dir/syntax.c.o.provides: CMakeFiles/compiler.dir/syntax.c.o.requires
	$(MAKE) -f CMakeFiles/compiler.dir/build.make CMakeFiles/compiler.dir/syntax.c.o.provides.build
.PHONY : CMakeFiles/compiler.dir/syntax.c.o.provides

CMakeFiles/compiler.dir/syntax.c.o.provides.build: CMakeFiles/compiler.dir/syntax.c.o


CMakeFiles/compiler.dir/table.c.o: CMakeFiles/compiler.dir/flags.make
CMakeFiles/compiler.dir/table.c.o: ../table.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/liuzheng/Public/ClionProjects/compiler/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/compiler.dir/table.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compiler.dir/table.c.o   -c /Users/liuzheng/Public/ClionProjects/compiler/table.c

CMakeFiles/compiler.dir/table.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compiler.dir/table.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/liuzheng/Public/ClionProjects/compiler/table.c > CMakeFiles/compiler.dir/table.c.i

CMakeFiles/compiler.dir/table.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compiler.dir/table.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/liuzheng/Public/ClionProjects/compiler/table.c -o CMakeFiles/compiler.dir/table.c.s

CMakeFiles/compiler.dir/table.c.o.requires:

.PHONY : CMakeFiles/compiler.dir/table.c.o.requires

CMakeFiles/compiler.dir/table.c.o.provides: CMakeFiles/compiler.dir/table.c.o.requires
	$(MAKE) -f CMakeFiles/compiler.dir/build.make CMakeFiles/compiler.dir/table.c.o.provides.build
.PHONY : CMakeFiles/compiler.dir/table.c.o.provides

CMakeFiles/compiler.dir/table.c.o.provides.build: CMakeFiles/compiler.dir/table.c.o


CMakeFiles/compiler.dir/mips.c.o: CMakeFiles/compiler.dir/flags.make
CMakeFiles/compiler.dir/mips.c.o: ../mips.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/liuzheng/Public/ClionProjects/compiler/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/compiler.dir/mips.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compiler.dir/mips.c.o   -c /Users/liuzheng/Public/ClionProjects/compiler/mips.c

CMakeFiles/compiler.dir/mips.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compiler.dir/mips.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/liuzheng/Public/ClionProjects/compiler/mips.c > CMakeFiles/compiler.dir/mips.c.i

CMakeFiles/compiler.dir/mips.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compiler.dir/mips.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/liuzheng/Public/ClionProjects/compiler/mips.c -o CMakeFiles/compiler.dir/mips.c.s

CMakeFiles/compiler.dir/mips.c.o.requires:

.PHONY : CMakeFiles/compiler.dir/mips.c.o.requires

CMakeFiles/compiler.dir/mips.c.o.provides: CMakeFiles/compiler.dir/mips.c.o.requires
	$(MAKE) -f CMakeFiles/compiler.dir/build.make CMakeFiles/compiler.dir/mips.c.o.provides.build
.PHONY : CMakeFiles/compiler.dir/mips.c.o.provides

CMakeFiles/compiler.dir/mips.c.o.provides.build: CMakeFiles/compiler.dir/mips.c.o


CMakeFiles/compiler.dir/optimize.c.o: CMakeFiles/compiler.dir/flags.make
CMakeFiles/compiler.dir/optimize.c.o: ../optimize.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/liuzheng/Public/ClionProjects/compiler/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/compiler.dir/optimize.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compiler.dir/optimize.c.o   -c /Users/liuzheng/Public/ClionProjects/compiler/optimize.c

CMakeFiles/compiler.dir/optimize.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compiler.dir/optimize.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/liuzheng/Public/ClionProjects/compiler/optimize.c > CMakeFiles/compiler.dir/optimize.c.i

CMakeFiles/compiler.dir/optimize.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compiler.dir/optimize.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/liuzheng/Public/ClionProjects/compiler/optimize.c -o CMakeFiles/compiler.dir/optimize.c.s

CMakeFiles/compiler.dir/optimize.c.o.requires:

.PHONY : CMakeFiles/compiler.dir/optimize.c.o.requires

CMakeFiles/compiler.dir/optimize.c.o.provides: CMakeFiles/compiler.dir/optimize.c.o.requires
	$(MAKE) -f CMakeFiles/compiler.dir/build.make CMakeFiles/compiler.dir/optimize.c.o.provides.build
.PHONY : CMakeFiles/compiler.dir/optimize.c.o.provides

CMakeFiles/compiler.dir/optimize.c.o.provides.build: CMakeFiles/compiler.dir/optimize.c.o


# Object files for target compiler
compiler_OBJECTS = \
"CMakeFiles/compiler.dir/main.c.o" \
"CMakeFiles/compiler.dir/lexical.c.o" \
"CMakeFiles/compiler.dir/error.c.o" \
"CMakeFiles/compiler.dir/syntax.c.o" \
"CMakeFiles/compiler.dir/table.c.o" \
"CMakeFiles/compiler.dir/mips.c.o" \
"CMakeFiles/compiler.dir/optimize.c.o"

# External object files for target compiler
compiler_EXTERNAL_OBJECTS =

compiler: CMakeFiles/compiler.dir/main.c.o
compiler: CMakeFiles/compiler.dir/lexical.c.o
compiler: CMakeFiles/compiler.dir/error.c.o
compiler: CMakeFiles/compiler.dir/syntax.c.o
compiler: CMakeFiles/compiler.dir/table.c.o
compiler: CMakeFiles/compiler.dir/mips.c.o
compiler: CMakeFiles/compiler.dir/optimize.c.o
compiler: CMakeFiles/compiler.dir/build.make
compiler: CMakeFiles/compiler.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/liuzheng/Public/ClionProjects/compiler/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking C executable compiler"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/compiler.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/compiler.dir/build: compiler

.PHONY : CMakeFiles/compiler.dir/build

CMakeFiles/compiler.dir/requires: CMakeFiles/compiler.dir/main.c.o.requires
CMakeFiles/compiler.dir/requires: CMakeFiles/compiler.dir/lexical.c.o.requires
CMakeFiles/compiler.dir/requires: CMakeFiles/compiler.dir/error.c.o.requires
CMakeFiles/compiler.dir/requires: CMakeFiles/compiler.dir/syntax.c.o.requires
CMakeFiles/compiler.dir/requires: CMakeFiles/compiler.dir/table.c.o.requires
CMakeFiles/compiler.dir/requires: CMakeFiles/compiler.dir/mips.c.o.requires
CMakeFiles/compiler.dir/requires: CMakeFiles/compiler.dir/optimize.c.o.requires

.PHONY : CMakeFiles/compiler.dir/requires

CMakeFiles/compiler.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/compiler.dir/cmake_clean.cmake
.PHONY : CMakeFiles/compiler.dir/clean

CMakeFiles/compiler.dir/depend:
	cd /Users/liuzheng/Public/ClionProjects/compiler/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/liuzheng/Public/ClionProjects/compiler /Users/liuzheng/Public/ClionProjects/compiler /Users/liuzheng/Public/ClionProjects/compiler/cmake-build-debug /Users/liuzheng/Public/ClionProjects/compiler/cmake-build-debug /Users/liuzheng/Public/ClionProjects/compiler/cmake-build-debug/CMakeFiles/compiler.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/compiler.dir/depend

