cmake_minimum_required(VERSION 3.20)

project(CallSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(proto)
add_subdirectory(Client)
add_subdirectory(Server)