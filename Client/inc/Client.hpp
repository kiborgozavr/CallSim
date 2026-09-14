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

    std::string generateUuid();

    CallSim::Message createMessage(CallSim::MessageType messageType, std::string fromUser, std::string payload);

    std::thread _consoleThread;
    RwWrapper _rwWrapper;

    std::string _clientId;
};

}  // namespace client
