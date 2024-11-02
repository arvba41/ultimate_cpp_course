cmake_minimum_required(VERSION 3.5.0)
project(casadi_101 VERSION 0.1.0 LANGUAGES C CXX)

find_package(Python)
set(casadi_DIR "${Python_SITEARCH}/casadi/cmake")
find_package(casadi REQUIRED)

find_package(nlohmann_json REQUIRED)

add_executable(casadi_101 main.cpp)
target_link_libraries(casadi_101 PRIVATE casadi nlohmann_json::nlohmann_json)
