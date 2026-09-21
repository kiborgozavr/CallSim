#include "Registered.hpp"

#include <iostream>

namespace client {

void Registered::enter() {
    std::cout << "You are registered on the server\n";
    std::cout << "Type \"exit\" to finish work\n";
}
std::unique_ptr<IState> Registered::transition(const CallSim::MessageType& msgType) { return nullptr; }

common::ClientStates Registered::getState() const { return common::ClientStates::registered; }
}  // namespace client