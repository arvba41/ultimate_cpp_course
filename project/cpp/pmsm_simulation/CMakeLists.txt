cmake_minimum_required(VERSION 3.5.0)
project(pmsm_simulation VERSION 0.1.0 LANGUAGES C CXX)

find_package(nlohmann_json REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(pmsm_simulation main.cc)
target_link_libraries(pmsm_simulation PRIVATE nlohmann_json::nlohmann_json Eigen3::Eigen)
target_compile_options(pmsm_simulation PRIVATE -Wall -Wextra -Wpedantic)

