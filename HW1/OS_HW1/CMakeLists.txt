cmake_minimum_required(VERSION 3.6)
project(OS_HW1)

set(CMAKE_CXX_STANDARD 98)

set(SOURCE_FILES main.cpp 8080emuCPP.h 8080emu.cpp gtuos.h gtuos.cpp)
add_executable(OS_HW1 ${SOURCE_FILES})