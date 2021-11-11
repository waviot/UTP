cmake_minimum_required(VERSION 3.10)
project(UTP C)
set(CMAKE_C_STANDARD 99)
add_executable(UTP main.c network.c network.h WVT_UTP.c WVT_UTP.h WVT_UTP_Types.h WVT_UTP_Memory.c WVT_UTP_Memory.h)