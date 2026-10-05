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
)

target_include_directories(Client PRIVATE
        inc
        ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(Client
        PRIVATE
        proto
)