# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.5.0)

# Define the project name, version, and languages used
project(PMSM_init VERSION 0.1.0 LANGUAGES C CXX)


# Find the required packages
find_package(nlohmann_json REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(fmt REQUIRED)

# ----------------------------------------------------------------
# Add the executable target and set the source files
# ----------------------------------------------------------------
add_executable(pmsm_simulation main.cc)

target_include_directories(pmsm_simulation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc) # Add the include directories for the target

target_link_libraries(pmsm_simulation PRIVATE nlohmann_json::nlohmann_json Eigen3::Eigen fmt::fmt) # Link the required libraries to the target

target_compile_features(pmsm_simulation PRIVATE cxx_std_20) # Set the C++ standard to C++20

target_compile_options(pmsm_simulation PRIVATE -Wall -Wextra -Wpedantic) # Set the compile options for the target

# ----------------------------------------------------------------
# Add the executable target and set the source files
# ----------------------------------------------------------------
add_executable(pmsm_simulation_test test.cc)

target_include_directories(pmsm_simulation_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc) 

target_link_libraries(pmsm_simulation_test PRIVATE nlohmann_json::nlohmann_json Eigen3::Eigen fmt::fmt) 

target_compile_features(pmsm_simulation_test PRIVATE cxx_std_20) 

target_compile_options(pmsm_simulation_test PRIVATE -Wall -Wextra -Wpedantic) 


