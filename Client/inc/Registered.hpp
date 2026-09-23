#pragma once

#include "IState.hpp"

namespace client {

class Registered : public IState {
public:
    std::unique_ptr<IState> transition(const CallSim::MessageType& msgType) override;

    void enter() override;

    [[nodiscard]] common::ClientStates getState() const override;

    void handleConsoleInput(Client& client, const std::string& line) override;
};
}  // namespace client
