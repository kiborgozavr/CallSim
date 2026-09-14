#include "../../inc/stateMachine/StateMachine.hpp"
#include "stateMachine/states/Connected.hpp"

#include <iostream>

namespace client {
StateMachine::StateMachine() :
        _state(std::make_unique<Connected>()) {
    _state->enter();
}
void StateMachine::next(const Events event) {
    if (auto newState = _state->transition(event); newState) {
        _state = std::move(newState);
        _state->enter();
        return;
    }
    std::cerr << "Undefined transition. Client stays in state: " << _state->get_name() << '\n';
}
}  // namespace client