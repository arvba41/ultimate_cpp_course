# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.5.0)

# Define the project name, version, and languages used
project(PMSM_init VERSION 0.1.0 LANGUAGES C CXX)

# Find the required packages
find_package(nlohmann_json REQUIRED)
find_package(Eigen3 REQUIRED)

# Include directories for header files
include_directories("src" "inc")


# Add the executable target
add_executable(pmsm_simulation main.cc)

# Link the required libraries to the target
target_link_libraries(pmsm_simulation PRIVATE nlohmann_json::nlohmann_json Eigen3::Eigen)

# Set the compile options for the target
target_compile_options(pmsm_simulation PRIVATE -Wall -Wextra -Wpedantic)

# Add a custom command to copy the executable to the results directory after building
# add_custom_command(TARGET pmsm_simulation POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pmsm_simulation> ../../results/cpp_simulation)

