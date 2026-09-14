#pragma once

#include "states/IState.hpp"

#include <memory>

namespace client {

class StateMachine {
public:
    StateMachine();

    void next(Events event);

private:
    std::unique_ptr<IState> _state;
};

}  // namespace client
