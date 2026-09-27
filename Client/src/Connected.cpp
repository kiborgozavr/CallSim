#include "Connected.hpp"
#include "Registered.hpp"

#include <iostream>

namespace client {

std::unique_ptr<IState> Connected::transition(const CallSim::Message& message) {
    if (message.type() == CallSim::Registered) {
        return std::make_unique<Registered>();
    }
    if (message.type() == CallSim::Error) {
        std::cerr << message.payload() << "\n";
        return nullptr;
    }
    return nullptr;
}

void Connected::enter(const CallSim::Message& message) {
    std::cout << "\n";
    std::cout << "You are connected, but not registered.\n";
}

common::ClientStates Connected::getState() const { return common::ClientStates::connected; }

void Connected::handleConsoleInput(Client& client, const std::string& line) {
    std::cout << "You are not registered on the server. Please, restart the application.\n";
}

}  // namespace client