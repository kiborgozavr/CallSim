#pragma once

#include "Events.hpp"

#include <memory>
#include <string>

namespace client {

class IState {
public:
    virtual ~IState() = default;

    virtual std::unique_ptr<IState> transition(Events) = 0;

    [[nodiscard]] virtual std::string getStateName() const = 0;

    virtual void enter() = 0;
};

}  // namespace client
