#include "../../inc/stateMachine/Connected.hpp"
#include "../../inc/stateMachine/Registered.hpp"

#include <iostream>

namespace client {

std::string Registered::getStateName() const { return "Registered"; }

void Registered::enter() { std::cout << "Client enter the state Registered\n"; }

std::unique_ptr<IState> Registered::transition(const Events event) {
    if (event == Unregister) {
        return std::make_unique<Connected>();
    }

    return nullptr;
}
}  // namespace client