add_executable(Server
        src/main.cpp
        src/RwWrapper.cpp
        src/Server.cpp

        inc/RwWrapper.hpp
        inc/Server.hpp
        inc/IRwWrapper.hpp
)

target_include_directories(Server PRIVATE
        inc
        ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(Server
        PRIVATE
        proto
)

# Tests

add_executable(ServerTest
        inc/IRwWrapper.hpp
        inc/Server.hpp

        src/Server.cpp

        test/MockRwWrapper.hpp
)

target_include_directories(ServerTest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/test
        ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(ServerTest
        PRIVATE
        GTest::gtest_main
        GTest::gmock
        proto
)

include(GoogleTest)

gtest_discover_tests(ServerTest)