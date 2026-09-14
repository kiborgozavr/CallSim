#include "../../../inc/stateMachine/states/Connected.hpp"
#include "../../../inc/stateMachine/states/Registered.hpp"

#include <iostream>

namespace client {

std::string Registered::get_name() const { return "Registered"; }

void Registered::enter() { std::cout << "Client enter the state Registered\n"; }

std::unique_ptr<IState> Registered::transition(const Events event) {
    if (event == Unregister) {
        return std::make_unique<Connected>();
    }

    return nullptr;
}
}  // namespace client