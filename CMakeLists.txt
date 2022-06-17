# CMakeList.txt : CMake project for ini, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("ini")

# Add source to this project's executable.
add_executable (ini "ini.cpp" "ini.h" "main.cpp")

# TODO: Add tests and install targets if needed.
