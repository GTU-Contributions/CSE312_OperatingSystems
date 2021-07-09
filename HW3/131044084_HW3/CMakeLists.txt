cmake_minimum_required(VERSION 3.6)
project(131044084_HW3)

set(CMAKE_CXX_STANDARD 98)

set(SOURCE_FILES main.cpp 8080emuCPP.h 8080emu.cpp gtuos.h gtuos.cpp memory.h memoryBase.h memory.cpp harddisk.h harddisk.cpp)
add_executable(131044084_HW3 ${SOURCE_FILES})