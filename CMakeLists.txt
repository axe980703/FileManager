cmake_minimum_required(VERSION 3.10)
project(FileManager)

set(CMAKE_CXX_STANDARD 11)

add_executable(FileManager main.c os_file.h task.c os_mem.h task2.cpp)