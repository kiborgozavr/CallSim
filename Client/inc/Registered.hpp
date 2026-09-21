#pragma once

#include "IState.hpp"

namespace client {

class Registered : public IState {
public:
    std::unique_ptr<IState> transition(const CallSim::MessageType& msgType) override;

    void enter() override;

    [[nodiscard]] common::ClientStates getState() const override;
};
}  // namespace client
