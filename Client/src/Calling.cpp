#include "Calling.hpp"

namespace client {

common::ClientStates Calling::transition(const CallSim::Message& message) {
    if (message.type() == CallSim::MessageType::Rejected) {
        std::cout << ">>> " << message.payload();
        return common::ClientStates::registered;
    }
    if (message.type() == CallSim::MessageType::Accepted) {
        std::cout << ">>> " << message.payload();
        return common::ClientStates::talking;
    }
    return common::ClientStates::calling;
}

[[nodiscard]] common::ClientStates Calling::getState() const { return common::ClientStates::calling; }

void Calling::enter(const CallSim::Message& message) {
    std::cout << "\n";
    std::cout << ">>> Calling...\n";
}

void Calling::handleConsoleInput(const std::string& line) {
    std::cout << ">>> Please, wait the addressee accept the call\n";
}

}  // namespace client