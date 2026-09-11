add_executable(Server
        src/main.cpp
        src/Server.cpp
        inc/Server.hpp
        src/RwWrapper.cpp
        inc/RwWrapper.hpp
)

target_include_directories(Server PRIVATE
        inc
)

target_link_libraries(Server
        PRIVATE
        proto
)