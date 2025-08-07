cmake_minimum_required(VERSION 3.16)
project(ShortestPathChallenge)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GEOS REQUIRED geos)

# Include directories
include_directories(${GEOS_INCLUDE_DIRS})
include_directories(include)

# Add executable
add_executable(shortest_path
    src/main.cpp
    src/geometry.cpp
    src/visibility_graph.cpp
    src/shortest_path.cpp
    src/json_parser.cpp
)

# Link libraries
target_link_libraries(shortest_path ${GEOS_LIBRARIES})
target_compile_options(shortest_path PRIVATE ${GEOS_CFLAGS_OTHER})

# Set output directory
set_target_properties(shortest_path PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy example data to build directory
configure_file(${CMAKE_SOURCE_DIR}/data/example_input.json 
               ${CMAKE_BINARY_DIR}/bin/example_input.json COPYONLY)