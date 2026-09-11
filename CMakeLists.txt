cmake_minimum_required(VERSION 3.20)

project(CallSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
find_package(Protobuf REQUIRED)

add_subdirectory(Client)
add_subdirectory(Server)
add_subdirectory(proto)