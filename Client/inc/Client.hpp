#pragma once

#include "IState.hpp"
#include "Message.pb.h"
#include "RwWrapper.hpp"

namespace client {
class Client {
public:
    explicit Client(unsigned int port, std::string clientName);

    ~Client();

    void start();

    void handleMessage(const CallSim::Message& message);

private:
    void stop();

    void readFromConsole();
    std::vector<std::string> parseInput(const std::string& line);
    void handleConsoleInput(const std::string& line, bool& continueReadingOut);

    void write(const CallSim::Message& message);

    void nextState(CallSim::MessageType msgType);

    CallSim::Message createMessage(CallSim::MessageType msgType, const std::string& toUser,
                                   const std::string& msgPayload);
    CallSim::Message createRegisterMessage(std::string fromUser);
    void setTimestamp(CallSim::Message& message);

    void printMessage(const CallSim::Message& message);

    RwWrapper _rwWrapper;

    std::string _clientName;
    std::unique_ptr<IState> _state;

    std::string _contactName;
};

}  // namespace client
