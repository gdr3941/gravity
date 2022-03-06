# Testings using Doctest

Unit testing files go in this directory for use with doctest
Each test file needs to include:
#include <doctest/doctest.h>
Each test file should just be a .cpp file that has the test macros

One file needs to have the testing main function. tests.cpp has this define included.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

Also when building, all source code files will be linked except for main.cpp
So no code should be in main that needs testing.


