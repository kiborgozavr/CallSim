#include "Calling.hpp"
#include "Client.hpp"
#include "Registered.hpp"

namespace client {

std::unique_ptr<IState> Calling::transition(const CallSim::MessageType& msgType) {
    switch (msgType) {
        case CallSim::MessageType::Rejected:
            return std::make_unique<Registered>();
        default:
            return nullptr;
    }
}

void Calling::enter() { std::cout << "Calling...\n"; }

void Calling::handleConsoleInput(Client& client, const std::string& line) {
    const CallSim::Message message = client.createMessage(CallSim::Text, client.getContactName(), line);
    client.write(message);
}

}  // namespace client