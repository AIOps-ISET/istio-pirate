file(GLOB source_files src/*.cpp src/*.hpp)
target_sources(pilot-app PRIVATE ${source_files})

find_package(kubernetes CONFIG REQUIRED COMPONENTS kubernetes)
target_link_libraries(pilot-app PRIVATE kubernetes::kubernetes)