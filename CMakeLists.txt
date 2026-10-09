cmake_minimum_required(VERSION 3.20)

project(CallSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
find_package(Protobuf REQUIRED)

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

FetchContent_MakeAvailable(googletest)

option(ENABLE_COVERAGE "Enable code coverage" OFF)

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

enable_testing()

add_subdirectory(Client)
add_subdirectory(Server)
add_subdirectory(proto)

