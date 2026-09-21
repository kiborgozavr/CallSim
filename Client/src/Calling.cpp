#include "Calling.hpp"
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

void Calling::enter() {
    std::cout << "Calling...\n";
    std::cout << "Type message to sent or \"end\" to finish the call\n";
}
}  // namespace client