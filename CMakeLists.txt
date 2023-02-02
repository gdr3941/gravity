# Template for Cmake C++ Projects
#
# Key files in addition to this one for setting up the project
# .dirs.local.el        sets the build and test commands
# src/.clang-format     options for clang-format
# build.sh              helper to setup and do initial builds
#
# C-c C-c   Compile and Run
# C-c C-b   Configure build (w/cmake+vcpkg) & compile, also set Debug or Release
# C-c C-t   Run tests
# C-c C-d   Debug 

# This CMake file compiles both a main executable (main) and a test runner (tests)
# To do this, it first builds a library from all the application code except main.
# It then uses this to link against the main executable and also the test executable.
#
cmake_minimum_required(VERSION 3.20)

# set the project name
project(Gravity)

# compiler options for application build
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# disabled due to errors compiling against system opengl?
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
# Enable sanitizers if desired to check for memory errors and undefined behavior
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fsanitize=address")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# project source and test files
set (source_dir "${PROJECT_SOURCE_DIR}/src/")
file (GLOB source_files "${source_dir}/*.cpp")
list(REMOVE_ITEM source_files "${source_dir}/main.cpp")  # pull out main so we can build lib
set (test_dir "${PROJECT_SOURCE_DIR}/tst/")
file (GLOB test_files "${test_dir}/*.cpp")

# Find dependencies, added to project from vcpkg.json manifest file in project root
# See output from cmake configure to get the actual naming needed
# Must also add resulting target to target_link_libraries
find_package(fmt REQUIRED)
find_package(TBB REQUIRED)
find_package(SFML REQUIRED system window graphics network audio)
find_package(imgui REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(doctest REQUIRED)

# create the application library target that has all code except main. This enables building the
# main and test executables seperately without compiling application twice.
add_library(lib ${source_files})

# add resource file to build directory
# configure_file("${PROJECT_SOURCE_DIR}/resources/arial.ttf" "${PROJECT_SOURCE_DIR}/build/arial.ttf" COPYONLY)

# build executable targets
add_executable(main ${PROJECT_SOURCE_DIR}/src/main.cpp)
add_executable(tests ${test_files})

# link library targets to executables
target_link_libraries(lib fmt::fmt TBB::tbb ${SFML_LIBRARIES} ImGui-SFML::ImGui-SFML)
target_link_libraries(main lib) 
target_link_libraries(tests PRIVATE doctest::doctest lib)
