add_executable(Server
        src/main.cpp
)

target_link_libraries(Server PRIVATE
        proto
)