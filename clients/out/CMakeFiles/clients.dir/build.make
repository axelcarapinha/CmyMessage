# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_SOURCE_DIR = /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/out

# Include any dependencies generated for this target.
include CMakeFiles/clients.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/clients.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/clients.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/clients.dir/flags.make

CMakeFiles/clients.dir/src/clients.c.o: CMakeFiles/clients.dir/flags.make
CMakeFiles/clients.dir/src/clients.c.o: /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/clients.c
CMakeFiles/clients.dir/src/clients.c.o: CMakeFiles/clients.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/clients.dir/src/clients.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/clients.dir/src/clients.c.o -MF CMakeFiles/clients.dir/src/clients.c.o.d -o CMakeFiles/clients.dir/src/clients.c.o -c /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/clients.c

CMakeFiles/clients.dir/src/clients.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/clients.dir/src/clients.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/clients.c > CMakeFiles/clients.dir/src/clients.c.i

CMakeFiles/clients.dir/src/clients.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/clients.dir/src/clients.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/clients.c -o CMakeFiles/clients.dir/src/clients.c.s

CMakeFiles/clients.dir/src/main.c.o: CMakeFiles/clients.dir/flags.make
CMakeFiles/clients.dir/src/main.c.o: /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/main.c
CMakeFiles/clients.dir/src/main.c.o: CMakeFiles/clients.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/clients.dir/src/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/clients.dir/src/main.c.o -MF CMakeFiles/clients.dir/src/main.c.o.d -o CMakeFiles/clients.dir/src/main.c.o -c /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/main.c

CMakeFiles/clients.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/clients.dir/src/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/main.c > CMakeFiles/clients.dir/src/main.c.i

CMakeFiles/clients.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/clients.dir/src/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/main.c -o CMakeFiles/clients.dir/src/main.c.s

CMakeFiles/clients.dir/src/utils/ftp_client.c.o: CMakeFiles/clients.dir/flags.make
CMakeFiles/clients.dir/src/utils/ftp_client.c.o: /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/utils/ftp_client.c
CMakeFiles/clients.dir/src/utils/ftp_client.c.o: CMakeFiles/clients.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/clients.dir/src/utils/ftp_client.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/clients.dir/src/utils/ftp_client.c.o -MF CMakeFiles/clients.dir/src/utils/ftp_client.c.o.d -o CMakeFiles/clients.dir/src/utils/ftp_client.c.o -c /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/utils/ftp_client.c

CMakeFiles/clients.dir/src/utils/ftp_client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/clients.dir/src/utils/ftp_client.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/utils/ftp_client.c > CMakeFiles/clients.dir/src/utils/ftp_client.c.i

CMakeFiles/clients.dir/src/utils/ftp_client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/clients.dir/src/utils/ftp_client.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/utils/ftp_client.c -o CMakeFiles/clients.dir/src/utils/ftp_client.c.s

CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.o: CMakeFiles/clients.dir/flags.make
CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.o: /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/utils/net_utils_tcp.c
CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.o: CMakeFiles/clients.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.o -MF CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.o.d -o CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.o -c /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/utils/net_utils_tcp.c

CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/utils/net_utils_tcp.c > CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.i

CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/src/utils/net_utils_tcp.c -o CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.s

# Object files for target clients
clients_OBJECTS = \
"CMakeFiles/clients.dir/src/clients.c.o" \
"CMakeFiles/clients.dir/src/main.c.o" \
"CMakeFiles/clients.dir/src/utils/ftp_client.c.o" \
"CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.o"

# External object files for target clients
clients_EXTERNAL_OBJECTS =

clients/clients: CMakeFiles/clients.dir/src/clients.c.o
clients/clients: CMakeFiles/clients.dir/src/main.c.o
clients/clients: CMakeFiles/clients.dir/src/utils/ftp_client.c.o
clients/clients: CMakeFiles/clients.dir/src/utils/net_utils_tcp.c.o
clients/clients: CMakeFiles/clients.dir/build.make
clients/clients: CMakeFiles/clients.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable clients/clients"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/clients.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/clients.dir/build: clients/clients
.PHONY : CMakeFiles/clients.dir/build

CMakeFiles/clients.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/clients.dir/cmake_clean.cmake
.PHONY : CMakeFiles/clients.dir/clean

CMakeFiles/clients.dir/depend:
	cd /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/out && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/out /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/out /home/heyuser/Desktop/TaskDemolisher/CmyMessage/clients/out/CMakeFiles/clients.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/clients.dir/depend

