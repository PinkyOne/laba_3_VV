cmake_minimum_required(VERSION 3.6)
project(Project6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")
#-D CMAKE_CXX_COMPILER=clang-omp++
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")

set(CMAKE_C_COMPILER /usr/bin/gcc-6)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
set(SOURCE_FILES
    Source.cpp test.cpp)

add_executable(Project6 ${SOURCE_FILES} test.cpp)
