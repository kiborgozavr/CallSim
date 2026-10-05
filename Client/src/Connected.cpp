#include "Client.hpp"
#include "Connected.hpp"

#include <iostream>

namespace client {

common::ClientStates Connected::transition(const CallSim::Message& message) {
    if (message.type() == CallSim::Registered) {
        return common::ClientStates::registered;
    }
    return common::ClientStates::connected;
}

void Connected::enter(const CallSim::Message& message) {
    std::cout << "\n";
    std::cout << ">>> You are connected, but not registered.\n";
}

common::ClientStates Connected::getState() const { return common::ClientStates::connected; }

void Connected::handleConsoleInput(const std::string& line) {
    std::cout << ">>> You are not registered on the server. Please, restart the application.\n";
}

}  // namespace client