add_executable(Server
        src/main.cpp
        src/Server.cpp
        inc/Server.hpp
        src/RwWrapper.cpp
        inc/RwWrapper.hpp
)

target_include_directories(Server PRIVATE
        inc
        ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(Server
        PRIVATE
        proto
)