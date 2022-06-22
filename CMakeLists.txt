cmake_minimum_required(VERSION 3.16)
project(RISC-V-Simulator)
include(CTest)

set(CMAKE_CXX_STANDARD 17)

set(SIMULATOR_INCLUDES
        include)

set(SIMULATOR_SOURCES
        src/main.cpp)

add_executable(code ${SIMULATOR_INCLUDES} ${SIMULATOR_SOURCES})
target_include_directories(code PRIVATE ${SIMULATOR_INCLUDES})
