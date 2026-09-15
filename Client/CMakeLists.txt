add_executable(Client
        src/main.cpp
        src/Client.cpp
        inc/Client.hpp
        inc/RwWrapper.hpp
        src/RwWrapper.cpp
        inc/stateMachine/IState.hpp
        inc/stateMachine/Events.hpp
        src/stateMachine/Connected.cpp
        inc/stateMachine/Connected.hpp
        src/stateMachine/Registered.cpp
        inc/stateMachine/Registered.hpp
)

target_include_directories(Client PRIVATE
        inc
)

target_link_libraries(Client
        PRIVATE
        proto
)