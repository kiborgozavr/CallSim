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

    void write(const std::string& message);

    void nextState(CallSim::MessageType msgType);

    CallSim::Message createMessage(CallSim::MessageType msgType, const std::string& msgPayload);
    CallSim::Message createRegisterMessage(std::string fromUser);
    void setTimestamp(CallSim::Message& message);

    void printMessage(const CallSim::Message& message);

    RwWrapper _rwWrapper;

    std::string _clientName;
    std::unique_ptr<IState> _state;
};

}  // namespace client
