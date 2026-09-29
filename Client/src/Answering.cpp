#include "Answering.hpp"
#include "Client.hpp"
#include "ClientStates.hpp"
#include "Registered.hpp"

namespace client {

std::unique_ptr<IState> Answering::transition(const CallSim::Message& message) {
    if (message.type() == CallSim::MessageType::Rejected) {
        return std::make_unique<Registered>();
    }
    return nullptr;
}

[[nodiscard]] common::ClientStates Answering::getState() const { return common::ClientStates::answering; }

void Answering::enter(const CallSim::Message& message) {
    std::cout << "\n";
    std::cout << "You have income call from " << message.from_user() << "\n";
    std::cout << "Type:\n";
    std::cout << "\t•\"yes\" to answer\n";
    std::cout << "\t•\"no\" to reject\n";
}

void Answering::handleConsoleInput(Client& client, const std::string& line) {
    if (line == "yes") {
        client.acceptCall();
    } else if (line == "no") {
        client.rejectCall();
    } else {
        std::cerr << "Unknown command. Please try again.\n";
    }
}
}  // namespace client