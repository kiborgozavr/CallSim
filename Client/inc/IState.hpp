#pragma once

#include "ClientStates.hpp"
#include "Message.pb.h"

namespace client {
class Client;

class IState {
public:
    explicit IState(Client& client) :
            _client(client) {}

    virtual ~IState() = default;

    virtual common::ClientStates transition(const CallSim::Message& message) = 0;

    virtual void enter(const CallSim::Message& message) = 0;

    [[nodiscard]] virtual common::ClientStates getState() const = 0;

    virtual void handleConsoleInput(const std::string& line) = 0;
    std::vector<std::string> parseInput(const std::string& line);

protected:
    Client& _client;
};

}  // namespace client
