#pragma once

#include "IConsoleReader.hpp"
#include "IState.hpp"
#include "Message.pb.h"
#include "RwWrapper.hpp"
#include "StateFactory.hpp"

namespace client {
class Client {
public:
    explicit Client(unsigned int port, std::string clientName);
    Client(std::unique_ptr<IRwWrapper> rwWrapper, std::unique_ptr<IConsoleReader> consoleReader,
           std::string clientName);  // for tests only

    void start();

    void handleMessage(const CallSim::Message& message);

    CallSim::Message createMessage(CallSim::MessageType msgType, const std::string& toUser = "",
                                   const std::string& msgPayload = "");

    void write(const CallSim::Message& message);

    void acceptCall();
    void rejectCall();
    void finishCall();

    [[nodiscard]] std::string& getContactName();
    void setContactName(const std::string& contactName);

    [[nodiscard]] common::ClientStates getState() const;

private:
    void readFromConsole();

    void nextState(const CallSim::Message& message);

    void setTimestamp(CallSim::Message& message);

    std::unique_ptr<IRwWrapper> _rwWrapper;
    std::unique_ptr<IConsoleReader> _consoleReader;
    StateFactory _stateFactory;

    std::string _clientName;
    std::unique_ptr<IState> _state;

    std::string _contactName;
};

}  // namespace client
