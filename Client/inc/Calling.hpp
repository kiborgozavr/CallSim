#pragma once

#include "IState.hpp"

namespace client {
class Calling : public IState {
public:
    std::unique_ptr<IState> transition(const CallSim::MessageType& msgType) override;

    void enter() override;

    [[nodiscard]] common::ClientStates getState() const override { return common::ClientStates::CALLING; }
};

}  // namespace client
