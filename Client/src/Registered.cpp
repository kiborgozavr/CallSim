#include "Client.hpp"
#include "Registered.hpp"

#include <iostream>

namespace client {

void Registered::enter(const CallSim::Message& message) {
    std::cout << "\n";
    std::cout << ">>> You are registered on the server\n";
    std::cout << ">>> Type:\n";
    std::cout << "\t•\"call + contact\" to call another user\n";
    std::cout << "\t•\"exit\" to finish work\n";
}

common::ClientStates Registered::getState() const { return common::ClientStates::registered; }

common::ClientStates Registered::transition(const CallSim::Message& message) {
    if (message.type() == CallSim::Calling) {
        return common::ClientStates::calling;
    }
    if (message.type() == CallSim::Answering) {
        return common::ClientStates::answering;
    }
    return common::ClientStates::registered;
}

void Registered::handleConsoleInput(const std::string& line) {
    const auto tokens = parseInput(line);

    if (tokens.size() != 2) {
        std::cerr << ">>> Invalid input. Please try again.\n";
        return;
    }

    if (tokens[0] == "call") {
        const std::string& contact = tokens[1];
        _client.setContactName(contact);

        const CallSim::Message message = _client.createMessage(CallSim::MessageType::Call, contact);
        _client.write(message);

    } else {
        std::cerr << ">>> Unknown command. Please try again.\n";
    }
}
}  // namespace client