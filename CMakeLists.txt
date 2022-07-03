cmake_minimum_required(VERSION 3.16)
project(RISC-V-Simulator)
include(CTest)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-O2")

if (DEFINED LAU_SHOW_ALL_DETAILS)
    set(CMAKE_CXX_FLAGS "-DLAU_TEST -DLAU_SHOW_REGISTER_DETAILS -DLAU_SHOW_ALL_DETAILS")
elseif (DEFINED LAU_SHOW_REGISTER_DETAILS)
    set(CMAKE_CXX_FLAGS "-DLAU_SHOW_REGISTER_DETAILS")
    if (DEFINED LAU_TEST)
        set(CMAKE_CXX_FLAGS "-DLAU_TEST")
    endif()
elseif (DEFINED LAU_TEST)
set(CMAKE_CXX_FLAGS "-DLAU_TEST")
endif()

set(SIMULATOR_INCLUDES
        include)

set(SIMULATOR_SOURCES
        src/ALU.cpp
        src/bus.cpp
        src/instructions.cpp
        src/main.cpp
        src/memory.cpp
        src/predictor.cpp
        src/register.cpp
        src/reorder_buffer.cpp
        src/reservation_station.cpp
        src/load_store_buffer.cpp)

add_executable(RISC-V-Simulator ${SIMULATOR_INCLUDES} ${SIMULATOR_SOURCES})
target_include_directories(RISC-V-Simulator PRIVATE ${SIMULATOR_INCLUDES})
