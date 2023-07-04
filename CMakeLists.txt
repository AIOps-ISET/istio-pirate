cmake_minimum_required(VERSION 3.25)
project(istio-pirate VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
endif ()

add_executable(pilot_app)
add_subdirectory(pilot)
find_package(kubernetes CONFIG REQUIRED COMPONENTS kubernetes)
target_link_libraries(pilot_app PRIVATE kubernetes::kubernetes)
