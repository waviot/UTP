cmake_minimum_required(VERSION 3.10)
project(UTP_Client)
set(CMAKE_CXX_STANDARD 17)
set(INCLUDE_DIR ../UTP/)
include_directories (${INCLUDE_DIR})
add_executable(UTP_Client   main.cpp Network.cpp Network.h 
                            UtpTool.cpp UtpTool.h ITransport.h 
                            UTool.h SerialPort.h SerialPort.cpp
                            SerialCommunication.h SerialCommunication.cpp)
target_link_libraries(UTP_Client stdc++fs)