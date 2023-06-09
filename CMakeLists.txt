cmake_minimum_required(VERSION 3.25)
project(istio-pirate VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
endif ()

add_executable(admission_controller)
add_executable(pilot-app)

add_subdirectory(yaml-inject)
add_subdirectory(libs/yaml-cpp)
add_subdirectory(pilot)

target_link_libraries(admission_controller PRIVATE yaml-cpp)
