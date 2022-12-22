# Template for Cmake C++ Projects
#
# Key files in addition to this one for setting up the project
# .dirs.local.el        sets the build and test commands
# src/.clang-format     options for clang-format
# build.sh              helper to setup and do initial builds
#
# C-c C-c   Build and Run
# C-c C-b   Build (and can change from Debug to Release if desired)
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
set(CMAKE_OSX_ARCHITECTURES "arm64")

# project source and test files
set (source_dir "${PROJECT_SOURCE_DIR}/src/")
file (GLOB source_files "${source_dir}/*.cpp")
list(REMOVE_ITEM source_files "${source_dir}/main.cpp")  # pull out main so we can build lib
set (test_dir "${PROJECT_SOURCE_DIR}/tst/")
file (GLOB test_files "${test_dir}/*.cpp")

# local project header only libs included manually
include_directories(include)

# find system installed libraries and config.
# Must also add resulting target to target_link_libraries
# find_package(SFML REQUIRED system window graphics network audio)
find_package(TBB REQUIRED)

# imgui-sfml needs to know where sfml cmake config is stored
set(SFML_DIR "${PROJECT_SOURCE_DIR}/extern/SFML/build")
# imgui-sfml needs to know where imgui is installed
set(IMGUI_DIR "${PROJECT_SOURCE_DIR}/include/imgui")

# Add local project cmake libraries (ie. don't use system installed)
# must also include the target (from add_library in cmake file) to link_libraries
# add_subdirectory(extern/fplus)
add_subdirectory(extern/SFML)
add_subdirectory(extern/imgui-sfml)

# also see this blog post: https://eliasdaler.github.io/using-cmake/

# create the application library target that has all code except main. This enables building the
# main and test executables seperately without compiling application twice.
add_library(lib ${source_files})

# add resource file to bin directory
configure_file("${PROJECT_SOURCE_DIR}/resources/arial.ttf" "${PROJECT_SOURCE_DIR}/build/arial.ttf" COPYONLY)

# build executable targets
add_executable(main ${PROJECT_SOURCE_DIR}/src/main.cpp)
add_executable(tests ${test_files})

# link library targets to executables
target_link_libraries(lib TBB::tbb ${SFML_LIBRARIES} ImGui-SFML::ImGui-SFML)
target_link_libraries(main lib)
target_link_libraries(tests PRIVATE lib)
