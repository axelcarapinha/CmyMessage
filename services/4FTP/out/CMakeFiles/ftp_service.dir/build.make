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
CMAKE_SOURCE_DIR = /home/heyuser/Desktop/CmyMessage/services/4FTP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/heyuser/Desktop/CmyMessage/services/4FTP/out

# Include any dependencies generated for this target.
include CMakeFiles/ftp_service.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ftp_service.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ftp_service.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ftp_service.dir/flags.make

CMakeFiles/ftp_service.dir/src/ftp_service.c.o: CMakeFiles/ftp_service.dir/flags.make
CMakeFiles/ftp_service.dir/src/ftp_service.c.o: /home/heyuser/Desktop/CmyMessage/services/4FTP/src/ftp_service.c
CMakeFiles/ftp_service.dir/src/ftp_service.c.o: CMakeFiles/ftp_service.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/heyuser/Desktop/CmyMessage/services/4FTP/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ftp_service.dir/src/ftp_service.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ftp_service.dir/src/ftp_service.c.o -MF CMakeFiles/ftp_service.dir/src/ftp_service.c.o.d -o CMakeFiles/ftp_service.dir/src/ftp_service.c.o -c /home/heyuser/Desktop/CmyMessage/services/4FTP/src/ftp_service.c

CMakeFiles/ftp_service.dir/src/ftp_service.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/ftp_service.dir/src/ftp_service.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/heyuser/Desktop/CmyMessage/services/4FTP/src/ftp_service.c > CMakeFiles/ftp_service.dir/src/ftp_service.c.i

CMakeFiles/ftp_service.dir/src/ftp_service.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/ftp_service.dir/src/ftp_service.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/heyuser/Desktop/CmyMessage/services/4FTP/src/ftp_service.c -o CMakeFiles/ftp_service.dir/src/ftp_service.c.s

CMakeFiles/ftp_service.dir/src/ftp_utils.c.o: CMakeFiles/ftp_service.dir/flags.make
CMakeFiles/ftp_service.dir/src/ftp_utils.c.o: /home/heyuser/Desktop/CmyMessage/services/4FTP/src/ftp_utils.c
CMakeFiles/ftp_service.dir/src/ftp_utils.c.o: CMakeFiles/ftp_service.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/heyuser/Desktop/CmyMessage/services/4FTP/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/ftp_service.dir/src/ftp_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ftp_service.dir/src/ftp_utils.c.o -MF CMakeFiles/ftp_service.dir/src/ftp_utils.c.o.d -o CMakeFiles/ftp_service.dir/src/ftp_utils.c.o -c /home/heyuser/Desktop/CmyMessage/services/4FTP/src/ftp_utils.c

CMakeFiles/ftp_service.dir/src/ftp_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/ftp_service.dir/src/ftp_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/heyuser/Desktop/CmyMessage/services/4FTP/src/ftp_utils.c > CMakeFiles/ftp_service.dir/src/ftp_utils.c.i

CMakeFiles/ftp_service.dir/src/ftp_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/ftp_service.dir/src/ftp_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/heyuser/Desktop/CmyMessage/services/4FTP/src/ftp_utils.c -o CMakeFiles/ftp_service.dir/src/ftp_utils.c.s

CMakeFiles/ftp_service.dir/src/main.c.o: CMakeFiles/ftp_service.dir/flags.make
CMakeFiles/ftp_service.dir/src/main.c.o: /home/heyuser/Desktop/CmyMessage/services/4FTP/src/main.c
CMakeFiles/ftp_service.dir/src/main.c.o: CMakeFiles/ftp_service.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/heyuser/Desktop/CmyMessage/services/4FTP/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/ftp_service.dir/src/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ftp_service.dir/src/main.c.o -MF CMakeFiles/ftp_service.dir/src/main.c.o.d -o CMakeFiles/ftp_service.dir/src/main.c.o -c /home/heyuser/Desktop/CmyMessage/services/4FTP/src/main.c

CMakeFiles/ftp_service.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/ftp_service.dir/src/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/heyuser/Desktop/CmyMessage/services/4FTP/src/main.c > CMakeFiles/ftp_service.dir/src/main.c.i

CMakeFiles/ftp_service.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/ftp_service.dir/src/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/heyuser/Desktop/CmyMessage/services/4FTP/src/main.c -o CMakeFiles/ftp_service.dir/src/main.c.s

CMakeFiles/ftp_service.dir/src/utils/hash_table.c.o: CMakeFiles/ftp_service.dir/flags.make
CMakeFiles/ftp_service.dir/src/utils/hash_table.c.o: /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/hash_table.c
CMakeFiles/ftp_service.dir/src/utils/hash_table.c.o: CMakeFiles/ftp_service.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/heyuser/Desktop/CmyMessage/services/4FTP/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/ftp_service.dir/src/utils/hash_table.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ftp_service.dir/src/utils/hash_table.c.o -MF CMakeFiles/ftp_service.dir/src/utils/hash_table.c.o.d -o CMakeFiles/ftp_service.dir/src/utils/hash_table.c.o -c /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/hash_table.c

CMakeFiles/ftp_service.dir/src/utils/hash_table.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/ftp_service.dir/src/utils/hash_table.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/hash_table.c > CMakeFiles/ftp_service.dir/src/utils/hash_table.c.i

CMakeFiles/ftp_service.dir/src/utils/hash_table.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/ftp_service.dir/src/utils/hash_table.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/hash_table.c -o CMakeFiles/ftp_service.dir/src/utils/hash_table.c.s

CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.o: CMakeFiles/ftp_service.dir/flags.make
CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.o: /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/net_utils_tcp.c
CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.o: CMakeFiles/ftp_service.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/heyuser/Desktop/CmyMessage/services/4FTP/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.o -MF CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.o.d -o CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.o -c /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/net_utils_tcp.c

CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/net_utils_tcp.c > CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.i

CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/net_utils_tcp.c -o CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.s

CMakeFiles/ftp_service.dir/src/utils/queue.c.o: CMakeFiles/ftp_service.dir/flags.make
CMakeFiles/ftp_service.dir/src/utils/queue.c.o: /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/queue.c
CMakeFiles/ftp_service.dir/src/utils/queue.c.o: CMakeFiles/ftp_service.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/heyuser/Desktop/CmyMessage/services/4FTP/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/ftp_service.dir/src/utils/queue.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ftp_service.dir/src/utils/queue.c.o -MF CMakeFiles/ftp_service.dir/src/utils/queue.c.o.d -o CMakeFiles/ftp_service.dir/src/utils/queue.c.o -c /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/queue.c

CMakeFiles/ftp_service.dir/src/utils/queue.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/ftp_service.dir/src/utils/queue.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/queue.c > CMakeFiles/ftp_service.dir/src/utils/queue.c.i

CMakeFiles/ftp_service.dir/src/utils/queue.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/ftp_service.dir/src/utils/queue.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/queue.c -o CMakeFiles/ftp_service.dir/src/utils/queue.c.s

CMakeFiles/ftp_service.dir/src/utils/service_utils.c.o: CMakeFiles/ftp_service.dir/flags.make
CMakeFiles/ftp_service.dir/src/utils/service_utils.c.o: /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/service_utils.c
CMakeFiles/ftp_service.dir/src/utils/service_utils.c.o: CMakeFiles/ftp_service.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/heyuser/Desktop/CmyMessage/services/4FTP/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/ftp_service.dir/src/utils/service_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ftp_service.dir/src/utils/service_utils.c.o -MF CMakeFiles/ftp_service.dir/src/utils/service_utils.c.o.d -o CMakeFiles/ftp_service.dir/src/utils/service_utils.c.o -c /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/service_utils.c

CMakeFiles/ftp_service.dir/src/utils/service_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/ftp_service.dir/src/utils/service_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/service_utils.c > CMakeFiles/ftp_service.dir/src/utils/service_utils.c.i

CMakeFiles/ftp_service.dir/src/utils/service_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/ftp_service.dir/src/utils/service_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/heyuser/Desktop/CmyMessage/services/4FTP/src/utils/service_utils.c -o CMakeFiles/ftp_service.dir/src/utils/service_utils.c.s

# Object files for target ftp_service
ftp_service_OBJECTS = \
"CMakeFiles/ftp_service.dir/src/ftp_service.c.o" \
"CMakeFiles/ftp_service.dir/src/ftp_utils.c.o" \
"CMakeFiles/ftp_service.dir/src/main.c.o" \
"CMakeFiles/ftp_service.dir/src/utils/hash_table.c.o" \
"CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.o" \
"CMakeFiles/ftp_service.dir/src/utils/queue.c.o" \
"CMakeFiles/ftp_service.dir/src/utils/service_utils.c.o"

# External object files for target ftp_service
ftp_service_EXTERNAL_OBJECTS =

ftp_service/ftp_service: CMakeFiles/ftp_service.dir/src/ftp_service.c.o
ftp_service/ftp_service: CMakeFiles/ftp_service.dir/src/ftp_utils.c.o
ftp_service/ftp_service: CMakeFiles/ftp_service.dir/src/main.c.o
ftp_service/ftp_service: CMakeFiles/ftp_service.dir/src/utils/hash_table.c.o
ftp_service/ftp_service: CMakeFiles/ftp_service.dir/src/utils/net_utils_tcp.c.o
ftp_service/ftp_service: CMakeFiles/ftp_service.dir/src/utils/queue.c.o
ftp_service/ftp_service: CMakeFiles/ftp_service.dir/src/utils/service_utils.c.o
ftp_service/ftp_service: CMakeFiles/ftp_service.dir/build.make
ftp_service/ftp_service: CMakeFiles/ftp_service.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/heyuser/Desktop/CmyMessage/services/4FTP/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking C executable ftp_service/ftp_service"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ftp_service.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ftp_service.dir/build: ftp_service/ftp_service
.PHONY : CMakeFiles/ftp_service.dir/build

CMakeFiles/ftp_service.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ftp_service.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ftp_service.dir/clean

CMakeFiles/ftp_service.dir/depend:
	cd /home/heyuser/Desktop/CmyMessage/services/4FTP/out && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/heyuser/Desktop/CmyMessage/services/4FTP /home/heyuser/Desktop/CmyMessage/services/4FTP /home/heyuser/Desktop/CmyMessage/services/4FTP/out /home/heyuser/Desktop/CmyMessage/services/4FTP/out /home/heyuser/Desktop/CmyMessage/services/4FTP/out/CMakeFiles/ftp_service.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/ftp_service.dir/depend

