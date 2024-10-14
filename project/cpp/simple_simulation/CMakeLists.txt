cmake_minimum_required(VERSION 3.5.0)
project(simple_simulation VERSION 0.1.0 LANGUAGES C CXX)

find_package(nlohmann_json REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(simple_simulation main.cc)
target_compile_options(simple_simulation PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(simple_simulation PRIVATE nlohmann_json::nlohmann_json)
