add_executable(Client
        src/main.cpp
)

target_link_libraries(Client PRIVATE
        proto
)