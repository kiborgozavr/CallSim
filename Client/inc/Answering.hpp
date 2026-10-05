#pragma once

#include "IState.hpp"

namespace client {

class Answering : public IState {
public:
    explicit Answering(Client& client) :
            IState(client) {}

    common::ClientStates transition(const CallSim::Message& message) override;

    void enter(const CallSim::Message& message) override;

    [[nodiscard]] common::ClientStates getState() const override;

    void handleConsoleInput(const std::string& line) override;
};

}  // namespace client
