#pragma once

#include "IState.hpp"

namespace client {

class Registered : public IState {
public:
    std::string get_name() const override;

    std::unique_ptr<IState> transition(Events event) override;

    void enter() override;
};

}  // namespace client
