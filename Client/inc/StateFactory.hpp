#pragma once
#include "IState.hpp"

namespace client {

class StateFactory {
public:
    std::unique_ptr<IState> getState(Client& client, common::ClientStates clientState);
};

}  // namespace client