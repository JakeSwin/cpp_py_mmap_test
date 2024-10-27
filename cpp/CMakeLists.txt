cmake_minimum_required(VERSION 3.22)

project(SHMReader VERSION 1.0
                  DESCRIPTION "Reads from shared memory test"
                  LANGUAGES CXX)

add_executable(reader main.cpp)