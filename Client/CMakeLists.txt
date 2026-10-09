add_executable(Client
        src/main.cpp
        src/Client.cpp
        inc/Client.hpp
        inc/RwWrapper.hpp
        src/RwWrapper.cpp
        inc/IState.hpp
        inc/Connected.hpp
        src/Connected.cpp
        src/Registered.cpp
        inc/Registered.hpp
        src/Calling.cpp
        inc/Calling.hpp
        src/IState.cpp
        inc/Answering.hpp
        src/Answering.cpp
        src/Talking.cpp
        inc/Talking.hpp
        src/StateFactory.cpp
        inc/StateFactory.hpp
        inc/IRwWrapper.hpp
        src/IConsoleReader.cpp
        inc/IConsoleReader.hpp
        inc/ConsoleReader.hpp
)

target_include_directories(Client PRIVATE
        inc
        ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(Client
        PRIVATE
        proto
)

# Tests

add_executable(ClientTest
        test/ClientTest.cpp
        test/MockRwWrapper.hpp
        test/MockConsoleReader.hpp
        src/Client.cpp
        inc/Client.hpp
        inc/RwWrapper.hpp
        src/RwWrapper.cpp
        inc/IState.hpp
        inc/Connected.hpp
        src/Connected.cpp
        src/Registered.cpp
        inc/Registered.hpp
        src/Calling.cpp
        inc/Calling.hpp
        src/IState.cpp
        inc/Answering.hpp
        src/Answering.cpp
        src/Talking.cpp
        inc/Talking.hpp
        src/StateFactory.cpp
        inc/StateFactory.hpp
        inc/IRwWrapper.hpp
        src/IConsoleReader.cpp
        inc/IConsoleReader.hpp
        inc/ConsoleReader.hpp
        test/AnsweringTest.cpp
        test/CallingTest.cpp
        test/ConnectedTest.cpp
        test/RegisteredTest.cpp
        test/TalkingTest.cpp
)

target_include_directories(ClientTest PRIVATE
        inc
        ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(ClientTest
        PRIVATE
        GTest::gtest_main
        GTest::gmock
        proto
)

include(GoogleTest)

gtest_discover_tests(ClientTest)