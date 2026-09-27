#include "Talking.hpp"

namespace client {
std::unique_ptr<IState> Talking::transition(const CallSim::Message& message) {
    // will add code here in the next task

    return nullptr;
}

[[nodiscard]] common::ClientStates Talking::getState() const { return common::ClientStates::talking; }

void Talking::enter(const CallSim::Message& message) {
    std::cout << "\n";
    std::cout << "You are talking to " << message.from_user() << "\n";
    std::cout << "Type message to send\n";
}

void Talking::handleConsoleInput(Client& client, const std::string& line) {
    // will add code here in the next task
}
}  // namespace client