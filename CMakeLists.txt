cmake_minimum_required (VERSION 2.8)
project(elang)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-c++98-compat -std=c++1y -O1") 

file(
    GLOB_RECURSE
    src_files
    src/elang/*.cpp
)

include_directories(include)
add_executable(elangc ${src_files})
