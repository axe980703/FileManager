cmake_minimum_required(VERSION 3.10)
project(FileManager C)

set(CMAKE_C_STANDARD 99)

add_executable(FileManager main.c os_file.h task.c)