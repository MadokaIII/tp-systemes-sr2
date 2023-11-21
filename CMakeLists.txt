cmake_minimum_required(VERSION 3.0)
project(MyProject)

# Add subdirectories
add_subdirectory(src/tp0)
add_subdirectory(src/tp1)
add_subdirectory(src/tp2)
add_subdirectory(src/tp3)

set(CMAKE_C_STANDARD 99)

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use AddressSanitizer and add warning flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g -fno-omit-frame-pointer -Wall -Wextra")
