#include "Answering.hpp"
#include "Client.hpp"
#include "ClientStates.hpp"

namespace client {

common::ClientStates Answering::transition(const CallSim::Message& message) {
    if (message.type() == CallSim::MessageType::Rejected) {
        return common::ClientStates::registered;
    }
    if (message.type() == CallSim::MessageType::Accepted) {
        return common::ClientStates::talking;
    }
    return common::ClientStates::answering;
}

[[nodiscard]] common::ClientStates Answering::getState() const { return common::ClientStates::answering; }

void Answering::enter(const CallSim::Message& message) {
    std::cout << "\n";
    std::cout << ">>> You have income call from " << message.from_user() << "\n";
    std::cout << ">>> Type:\n";
    std::cout << "\t•\"yes\" to answer\n";
    std::cout << "\t•\"no\" to reject\n";
}

void Answering::handleConsoleInput(const std::string& line) {
    if (line == "yes") {
        _client.acceptCall();
    } else if (line == "no") {
        _client.rejectCall();
    } else {
        std::cerr << ">>> Unknown command. Please try again.\n";
    }
}
}  // namespace client