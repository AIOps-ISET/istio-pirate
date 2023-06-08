cmake_policy(SET CMP0048 NEW)
project(istio-pirate VERSION 0.0.1 LANGUAGES CXX)
cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(yaml-inject)

add_subdirectory(yaml-inject)
add_subdirectory(libs/yaml-cpp)
target_link_libraries(yaml-inject yaml-cpp)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
endif ()
