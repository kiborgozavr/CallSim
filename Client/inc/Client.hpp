#pragma once

#include "Message.pb.h"
#include "RwWrapper.hpp"
#include "stateMachine/StateMachine.hpp"

namespace client {
class Client {
public:
    explicit Client(unsigned int port);

    void start();

private:
    void stop();

    void readFromConsole();

    void starExchangingMessages();

    void write(const std::string &msg);

    CallSim::Message createMessage(CallSim::MessageType msgType, std::string msgPayload);

    std::thread _consoleThread;
    RwWrapper _rwWrapper;

    std::string _clientId;

    StateMachine _stateMachine;
};

}  // namespace client
