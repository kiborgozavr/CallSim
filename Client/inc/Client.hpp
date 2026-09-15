#pragma once

#include "Message.pb.h"
#include "RwWrapper.hpp"
#include "stateMachine/IState.hpp"

namespace client {
class Client {
public:
    explicit Client(unsigned int port);

    void start();

private:
    void stop();

    void readFromConsole();

    void startExchangingMessages();

    void write(const std::string& msg);

    void nextState(Events event);

    CallSim::Message createMessage(CallSim::MessageType msgType, std::string msgPayload);

    std::thread _consoleThread;
    RwWrapper _rwWrapper;

    std::string _clientId;
    std::unique_ptr<IState> _state;
};

}  // namespace client
