#include "Connected.hpp"
#include "Registered.hpp"

#include <iostream>

namespace client {

std::unique_ptr<IState> Connected::transition(const CallSim::MessageType& msgType) {
    if (msgType == CallSim::Registered) {
        return std::make_unique<Registered>();
    }
    return nullptr;
}

void Connected::enter() { std::cout << "You are connected, but not registered.\n"; }

common::ClientStates Connected::getState() const { return common::ClientStates::connected; }

void Connected::handleConsoleInput(Client& client, const std::string& line) {
    std::cout << "You are not registered on the server. Please, restart the application.\n";
}

}  // namespace client