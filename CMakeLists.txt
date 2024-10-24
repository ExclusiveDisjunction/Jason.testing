cmake_minimum_required(VERSION 3.29)
project(jason_testing)

set(CMAKE_CXX_STANDARD 20)

add_executable(jason_testing BinaryUnit.cpp BinaryUnit.h main.cpp PackagePager.cpp PackagePager.h FileHandle.cpp FileHandle.h)
