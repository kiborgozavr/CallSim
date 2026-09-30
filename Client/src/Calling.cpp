#include "Calling.hpp"
#include "Client.hpp"
#include "Registered.hpp"
#include "Talking.hpp"

namespace client {

std::unique_ptr<IState> Calling::transition(const CallSim::Message& message) {
    if (message.type() == CallSim::MessageType::Rejected) {
        std::cout << message.payload();
        return std::make_unique<Registered>();
    }
    if (message.type() == CallSim::MessageType::Accepted) {
        std::cout << message.payload();
        return std::make_unique<Talking>();
    }
    return nullptr;
}

[[nodiscard]] common::ClientStates Calling::getState() const { return common::ClientStates::calling; }

void Calling::enter(const CallSim::Message& message) {
    std::cout << "\n";
    std::cout << "Calling...\n";
}

void Calling::handleConsoleInput(Client& client, const std::string& line) {
    std::cout << "Please, wait the addressee accept the call\n";
}

}  // namespace client