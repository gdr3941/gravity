# Template for Cmake C++ Projects
#
# Key files in addition to this one for setting up the project
# .dirs.local.el        sets the build and test commands
# src/.clang-format     options for clang-format
# conanfile.txt         dependancies
# build.sh              helper to setup and do initial builds
# ~/.conan/profiles     profiles for defining how project deps are compiled
#
# C-c C-c   Build and Run
# C-c C-b   Build (and can change from Debug to Release if desired)
# C-c C-t   Run tests
# C-c C-d   Debug 

# This CMake file compiles both a main executable (main) and a test runner (tests)
# To do this, it first builds a library from all the application code except main.
# It then uses this to link against the main executable and also the test executable.
#
# Dependencies are managed by Conan. See conanfile.txt.

cmake_minimum_required(VERSION 3.20)

# set the project name
project(Gravity)

# compiler options for application build
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
# Enable sanitizers if desired to check for memory errors and undefined behavior
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fsanitize=address")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# project source and test files
set (source_dir "${PROJECT_SOURCE_DIR}/src/")
file (GLOB source_files "${source_dir}/*.cpp")
list(REMOVE_ITEM source_files "${source_dir}/main.cpp")  # pull out main so we can build lib
set (test_dir "${PROJECT_SOURCE_DIR}/tst/")
file (GLOB test_files "${test_dir}/*.cpp")

# local project header libs included manually. (Prefer using Conan)
include_directories(include)

# conan setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# find system installed libraries and config. (Prefer using Conan)
# Must also add resulting target to target_link_libraries
# find_package(SFML REQUIRED system window graphics network audio)
find_package(TBB REQUIRED)  # not working from conan

# Alternative: add in-project source code libs that are cmake based
# must also include the target (from add_library in cmake file) to link_libraries
# add_subdirectory(extern/fplus)
# also see this blog post: https://eliasdaler.github.io/using-cmake/

# create the application library target that has all code except main. This enables building the
# main and test executables seperately without compiling application twice.
add_library(lib ${source_files})

# add resource file to bin directory
configure_file("${PROJECT_SOURCE_DIR}/resources/arial.ttf" "${PROJECT_SOURCE_DIR}/build/bin/arial.ttf" COPYONLY)

# build executable targets
add_executable(main ${PROJECT_SOURCE_DIR}/src/main.cpp)
add_executable(tests ${test_files})

# link library targets to executables
target_link_libraries(lib ${CONAN_LIBS} TBB::tbb)
target_link_libraries(main lib)
target_link_libraries(tests PRIVATE lib)
