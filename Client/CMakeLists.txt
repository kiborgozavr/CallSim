add_executable(Client
        src/main.cpp
        src/Client.cpp
        inc/Client.hpp
        inc/RwWrapper.hpp
        src/RwWrapper.cpp
        inc/stateMachine/states/IState.hpp
        inc/stateMachine/Events.hpp
        src/stateMachine/StateMachine.cpp
        inc/stateMachine/StateMachine.hpp
        src/stateMachine/states/Connected.cpp
        inc/stateMachine/states/Connected.hpp
        src/stateMachine/states/Registered.cpp
        inc/stateMachine/states/Registered.hpp
)

target_include_directories(Client PRIVATE
        inc
)

target_link_libraries(Client
        PRIVATE
        proto
)