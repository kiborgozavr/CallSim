#pragma once

#include "Message.pb.h"
#include "RwWrapper.hpp"

namespace client {
class Client {
public:
    explicit Client(unsigned int port);

    void start();

private:
    void stop();

    void readFromConsole();

    void write(const std::string &msg);

    CallSim::Message createMessage(CallSim::MessageType msgType, std::string msgPayload);

    std::thread _consoleThread;
    RwWrapper _rwWrapper;

    std::string _clientId;
};

}  // namespace client
