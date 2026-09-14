#include "../../../inc/stateMachine/states/Connected.hpp"
#include "stateMachine/states/Registered.hpp"

#include <iostream>

namespace client {

std::string Connected::get_name() const { return "Connected"; }

void Connected::enter() { std::cout << "Client enter the state Connected\n"; }

std::unique_ptr<IState> Connected::transition(const Events event) {
    if (event == Register) {
        return std::make_unique<Registered>();
    }

    return nullptr;
}

}  // namespace client