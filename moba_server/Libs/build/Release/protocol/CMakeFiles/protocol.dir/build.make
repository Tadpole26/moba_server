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
CMAKE_SOURCE_DIR = /root/Duncan/project/roll_server/Libs/protocol

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/Duncan/project/roll_server/Libs/build/Release/protocol

# Include any dependencies generated for this target.
include CMakeFiles/protocol.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/protocol.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/protocol.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/protocol.dir/flags.make

CMakeFiles/protocol.dir/msg_common.pb.cc.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/msg_common.pb.cc.o: /root/Duncan/project/roll_server/Libs/protocol/msg_common.pb.cc
CMakeFiles/protocol.dir/msg_common.pb.cc.o: CMakeFiles/protocol.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/Duncan/project/roll_server/Libs/build/Release/protocol/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/protocol.dir/msg_common.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/protocol.dir/msg_common.pb.cc.o -MF CMakeFiles/protocol.dir/msg_common.pb.cc.o.d -o CMakeFiles/protocol.dir/msg_common.pb.cc.o -c /root/Duncan/project/roll_server/Libs/protocol/msg_common.pb.cc

CMakeFiles/protocol.dir/msg_common.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/msg_common.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/Duncan/project/roll_server/Libs/protocol/msg_common.pb.cc > CMakeFiles/protocol.dir/msg_common.pb.cc.i

CMakeFiles/protocol.dir/msg_common.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/msg_common.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/Duncan/project/roll_server/Libs/protocol/msg_common.pb.cc -o CMakeFiles/protocol.dir/msg_common.pb.cc.s

CMakeFiles/protocol.dir/msg_module.pb.cc.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/msg_module.pb.cc.o: /root/Duncan/project/roll_server/Libs/protocol/msg_module.pb.cc
CMakeFiles/protocol.dir/msg_module.pb.cc.o: CMakeFiles/protocol.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/Duncan/project/roll_server/Libs/build/Release/protocol/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/protocol.dir/msg_module.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/protocol.dir/msg_module.pb.cc.o -MF CMakeFiles/protocol.dir/msg_module.pb.cc.o.d -o CMakeFiles/protocol.dir/msg_module.pb.cc.o -c /root/Duncan/project/roll_server/Libs/protocol/msg_module.pb.cc

CMakeFiles/protocol.dir/msg_module.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/msg_module.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/Duncan/project/roll_server/Libs/protocol/msg_module.pb.cc > CMakeFiles/protocol.dir/msg_module.pb.cc.i

CMakeFiles/protocol.dir/msg_module.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/msg_module.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/Duncan/project/roll_server/Libs/protocol/msg_module.pb.cc -o CMakeFiles/protocol.dir/msg_module.pb.cc.s

CMakeFiles/protocol.dir/msg_module_http.pb.cc.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/msg_module_http.pb.cc.o: /root/Duncan/project/roll_server/Libs/protocol/msg_module_http.pb.cc
CMakeFiles/protocol.dir/msg_module_http.pb.cc.o: CMakeFiles/protocol.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/Duncan/project/roll_server/Libs/build/Release/protocol/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/protocol.dir/msg_module_http.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/protocol.dir/msg_module_http.pb.cc.o -MF CMakeFiles/protocol.dir/msg_module_http.pb.cc.o.d -o CMakeFiles/protocol.dir/msg_module_http.pb.cc.o -c /root/Duncan/project/roll_server/Libs/protocol/msg_module_http.pb.cc

CMakeFiles/protocol.dir/msg_module_http.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/msg_module_http.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/Duncan/project/roll_server/Libs/protocol/msg_module_http.pb.cc > CMakeFiles/protocol.dir/msg_module_http.pb.cc.i

CMakeFiles/protocol.dir/msg_module_http.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/msg_module_http.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/Duncan/project/roll_server/Libs/protocol/msg_module_http.pb.cc -o CMakeFiles/protocol.dir/msg_module_http.pb.cc.s

CMakeFiles/protocol.dir/msg_module_login.pb.cc.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/msg_module_login.pb.cc.o: /root/Duncan/project/roll_server/Libs/protocol/msg_module_login.pb.cc
CMakeFiles/protocol.dir/msg_module_login.pb.cc.o: CMakeFiles/protocol.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/Duncan/project/roll_server/Libs/build/Release/protocol/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/protocol.dir/msg_module_login.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/protocol.dir/msg_module_login.pb.cc.o -MF CMakeFiles/protocol.dir/msg_module_login.pb.cc.o.d -o CMakeFiles/protocol.dir/msg_module_login.pb.cc.o -c /root/Duncan/project/roll_server/Libs/protocol/msg_module_login.pb.cc

CMakeFiles/protocol.dir/msg_module_login.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/msg_module_login.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/Duncan/project/roll_server/Libs/protocol/msg_module_login.pb.cc > CMakeFiles/protocol.dir/msg_module_login.pb.cc.i

CMakeFiles/protocol.dir/msg_module_login.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/msg_module_login.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/Duncan/project/roll_server/Libs/protocol/msg_module_login.pb.cc -o CMakeFiles/protocol.dir/msg_module_login.pb.cc.s

CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.o: /root/Duncan/project/roll_server/Libs/protocol/msg_module_servercommon.pb.cc
CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.o: CMakeFiles/protocol.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/Duncan/project/roll_server/Libs/build/Release/protocol/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.o -MF CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.o.d -o CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.o -c /root/Duncan/project/roll_server/Libs/protocol/msg_module_servercommon.pb.cc

CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/Duncan/project/roll_server/Libs/protocol/msg_module_servercommon.pb.cc > CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.i

CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/Duncan/project/roll_server/Libs/protocol/msg_module_servercommon.pb.cc -o CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.s

CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.o: /root/Duncan/project/roll_server/Libs/protocol/msg_module_serverinner.pb.cc
CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.o: CMakeFiles/protocol.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/Duncan/project/roll_server/Libs/build/Release/protocol/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.o -MF CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.o.d -o CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.o -c /root/Duncan/project/roll_server/Libs/protocol/msg_module_serverinner.pb.cc

CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/Duncan/project/roll_server/Libs/protocol/msg_module_serverinner.pb.cc > CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.i

CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/Duncan/project/roll_server/Libs/protocol/msg_module_serverinner.pb.cc -o CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.s

CMakeFiles/protocol.dir/result_code.pb.cc.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/result_code.pb.cc.o: /root/Duncan/project/roll_server/Libs/protocol/result_code.pb.cc
CMakeFiles/protocol.dir/result_code.pb.cc.o: CMakeFiles/protocol.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/Duncan/project/roll_server/Libs/build/Release/protocol/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/protocol.dir/result_code.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/protocol.dir/result_code.pb.cc.o -MF CMakeFiles/protocol.dir/result_code.pb.cc.o.d -o CMakeFiles/protocol.dir/result_code.pb.cc.o -c /root/Duncan/project/roll_server/Libs/protocol/result_code.pb.cc

CMakeFiles/protocol.dir/result_code.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/result_code.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/Duncan/project/roll_server/Libs/protocol/result_code.pb.cc > CMakeFiles/protocol.dir/result_code.pb.cc.i

CMakeFiles/protocol.dir/result_code.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/result_code.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/Duncan/project/roll_server/Libs/protocol/result_code.pb.cc -o CMakeFiles/protocol.dir/result_code.pb.cc.s

CMakeFiles/protocol.dir/server_common.pb.cc.o: CMakeFiles/protocol.dir/flags.make
CMakeFiles/protocol.dir/server_common.pb.cc.o: /root/Duncan/project/roll_server/Libs/protocol/server_common.pb.cc
CMakeFiles/protocol.dir/server_common.pb.cc.o: CMakeFiles/protocol.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/Duncan/project/roll_server/Libs/build/Release/protocol/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/protocol.dir/server_common.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/protocol.dir/server_common.pb.cc.o -MF CMakeFiles/protocol.dir/server_common.pb.cc.o.d -o CMakeFiles/protocol.dir/server_common.pb.cc.o -c /root/Duncan/project/roll_server/Libs/protocol/server_common.pb.cc

CMakeFiles/protocol.dir/server_common.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protocol.dir/server_common.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/Duncan/project/roll_server/Libs/protocol/server_common.pb.cc > CMakeFiles/protocol.dir/server_common.pb.cc.i

CMakeFiles/protocol.dir/server_common.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protocol.dir/server_common.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/Duncan/project/roll_server/Libs/protocol/server_common.pb.cc -o CMakeFiles/protocol.dir/server_common.pb.cc.s

# Object files for target protocol
protocol_OBJECTS = \
"CMakeFiles/protocol.dir/msg_common.pb.cc.o" \
"CMakeFiles/protocol.dir/msg_module.pb.cc.o" \
"CMakeFiles/protocol.dir/msg_module_http.pb.cc.o" \
"CMakeFiles/protocol.dir/msg_module_login.pb.cc.o" \
"CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.o" \
"CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.o" \
"CMakeFiles/protocol.dir/result_code.pb.cc.o" \
"CMakeFiles/protocol.dir/server_common.pb.cc.o"

# External object files for target protocol
protocol_EXTERNAL_OBJECTS =

/root/Duncan/project/roll_server/Bin/Libs/linux/libprotocol.a: CMakeFiles/protocol.dir/msg_common.pb.cc.o
/root/Duncan/project/roll_server/Bin/Libs/linux/libprotocol.a: CMakeFiles/protocol.dir/msg_module.pb.cc.o
/root/Duncan/project/roll_server/Bin/Libs/linux/libprotocol.a: CMakeFiles/protocol.dir/msg_module_http.pb.cc.o
/root/Duncan/project/roll_server/Bin/Libs/linux/libprotocol.a: CMakeFiles/protocol.dir/msg_module_login.pb.cc.o
/root/Duncan/project/roll_server/Bin/Libs/linux/libprotocol.a: CMakeFiles/protocol.dir/msg_module_servercommon.pb.cc.o
/root/Duncan/project/roll_server/Bin/Libs/linux/libprotocol.a: CMakeFiles/protocol.dir/msg_module_serverinner.pb.cc.o
/root/Duncan/project/roll_server/Bin/Libs/linux/libprotocol.a: CMakeFiles/protocol.dir/result_code.pb.cc.o
/root/Duncan/project/roll_server/Bin/Libs/linux/libprotocol.a: CMakeFiles/protocol.dir/server_common.pb.cc.o
/root/Duncan/project/roll_server/Bin/Libs/linux/libprotocol.a: CMakeFiles/protocol.dir/build.make
/root/Duncan/project/roll_server/Bin/Libs/linux/libprotocol.a: CMakeFiles/protocol.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/Duncan/project/roll_server/Libs/build/Release/protocol/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Linking CXX static library /root/Duncan/project/roll_server/Bin/Libs/linux/libprotocol.a"
	$(CMAKE_COMMAND) -P CMakeFiles/protocol.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/protocol.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/protocol.dir/build: /root/Duncan/project/roll_server/Bin/Libs/linux/libprotocol.a
.PHONY : CMakeFiles/protocol.dir/build

CMakeFiles/protocol.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/protocol.dir/cmake_clean.cmake
.PHONY : CMakeFiles/protocol.dir/clean

CMakeFiles/protocol.dir/depend:
	cd /root/Duncan/project/roll_server/Libs/build/Release/protocol && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/Duncan/project/roll_server/Libs/protocol /root/Duncan/project/roll_server/Libs/protocol /root/Duncan/project/roll_server/Libs/build/Release/protocol /root/Duncan/project/roll_server/Libs/build/Release/protocol /root/Duncan/project/roll_server/Libs/build/Release/protocol/CMakeFiles/protocol.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/protocol.dir/depend

