#pragma once

#include "IState.hpp"

namespace client {

class Registered : public IState {
public:
    [[nodiscard]] std::string getStateName() const override;

    std::unique_ptr<IState> transition(Events event) override;

    void enter() override;
};
}  // namespace client
