#include "Calling.hpp"
#include "Client.hpp"
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

void Registered::handleConsoleInput(Client& client, const std::string& line) {
    const auto tokens = parseInput(line);

    if (tokens.empty() || tokens.size() > 2) {
        std::cerr << "Invalid input. Please try again.\n";
    }

    if (tokens[0] == "call") {
        const std::string& contact = tokens[1];
        client.setContactName(contact);

        const CallSim::Message message = client.createMessage(CallSim::MessageType::Call, contact);

        client.write(message);

    } else {
        std::cerr << "Unknown command. Please try again.\n";
    }
}
}  // namespace client