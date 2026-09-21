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

void Connected::enter() { std::cout << "Starting work...\n"; }

common::ClientStates Connected::getState() const { return common::ClientStates::connected; }
}  // namespace client