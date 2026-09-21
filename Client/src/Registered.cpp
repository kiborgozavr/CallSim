#include "Calling.hpp"
#include "Registered.hpp"

#include <iostream>

namespace client {

void Registered::enter() {
    std::cout << "You are registered on the server\n";
    std::cout << "Type:\n";
    std::cout << "\t•\"call + contact\" to call another user\n";
    std::cout << "\t•\"exit\" to finish work\n";
}

common::ClientStates Registered::getState() const { return common::ClientStates::registered; }

std::unique_ptr<IState> Registered::transition(const CallSim::MessageType& msgType) {
    if (msgType == CallSim::Calling) {
        return std::make_unique<Calling>();
    }

    return nullptr;
}
}  // namespace client