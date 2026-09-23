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

    CallSim::Message createMessage(CallSim::MessageType msgType, const std::string& toUser = "",
                                   const std::string& msgPayload = "");

    void write(const CallSim::Message& message);

    [[nodiscard]] std::string& getContactName();
    void setContactName(const std::string& contactName);

private:
    void stop();

    void readFromConsole();

    void nextState(CallSim::MessageType msgType);

    void setTimestamp(CallSim::Message& message);

    RwWrapper _rwWrapper;

    std::string _clientName;
    std::unique_ptr<IState> _state;

    std::string _contactName;
};

}  // namespace client
