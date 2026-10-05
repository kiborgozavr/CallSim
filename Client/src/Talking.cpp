#include "Client.hpp"
#include "Talking.hpp"

#include <iostream>

namespace client {

void Talking::enter(const CallSim::Message& message) {
    std::cout << "\n";
    std::cout << ">>> You are in chat with " << message.from_user() << "\n";
    std::cout << ">>> Type:\n";
    std::cout << "\tyour message to send\n";
    std::cout << "\t•\"end\" to finish call\n";
}

common::ClientStates Talking::getState() const { return common::ClientStates::talking; }

common::ClientStates Talking::transition(const CallSim::Message& message) {
    if (message.type() == CallSim::MessageType::Ended) {
        return common::ClientStates::registered;
    }
    return common::ClientStates::talking;
}

void Talking::handleConsoleInput(const std::string& line) {
    if (line.empty()) {
        std::cerr << ">>> Invalid input. Please try again.\n";
        return;
    }

    if (line == "end") {
        _client.finishCall();
        return;
    }

    _client.write(_client.createMessage(CallSim::Text, _client.getContactName(), line));
}
}  // namespace client