#include "Client.hpp"
#include "Registered.hpp"
#include "Talking.hpp"

#include <iostream>

namespace client {

void Talking::enter(const CallSim::Message& message) {
    std::cout << "\n";
    std::cout << "You are in chat with " << message.from_user() << "\n";
    std::cout << "Type:\n";
    std::cout << "\tyour message to send\n";
    std::cout << "\t•\"end\" to finish call\n";
}

common::ClientStates Talking::getState() const { return common::ClientStates::talking; }

std::unique_ptr<IState> Talking::transition(const CallSim::Message& message) {
    if (message.type() == CallSim::MessageType::Ended) {
        return std::make_unique<Registered>();
    }
    return nullptr;
}

void Talking::handleConsoleInput(Client& client, const std::string& line) {
    if (line.empty()) {
        std::cerr << "Invalid input. Please try again.\n";
        return;
    }

    if (line == "end") {
        client.finishCall();
        return;
    }

    client.write(client.createMessage(CallSim::Text, client.getContactName(), line));
}
}  // namespace client