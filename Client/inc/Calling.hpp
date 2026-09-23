#pragma once

#include "IState.hpp"

namespace client {
class Calling : public IState {
public:
    std::unique_ptr<IState> transition(const CallSim::MessageType& msgType) override;

    void enter() override;

    [[nodiscard]] common::ClientStates getState() const override { return common::ClientStates::calling; }

    void handleConsoleInput(Client& client, const std::string& line) override;

};

}  // namespace client
