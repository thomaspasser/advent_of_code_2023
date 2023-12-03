cmake_minimum_required(VERSION 3.10)

set(PROJECT_N day2)
project(${PROJECT_N} VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/*.cpp)

message("src files:")
foreach(file ${SRCS})
        message(STATUS ${file})
endforeach()

add_executable(${PROJECT_N} ${SRCS})
